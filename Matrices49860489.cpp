// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define ENEMY_NUM 200


// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface



						// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_block;    // the pointer to the sprite



									 // function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);


// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };


//기본 클래스 
class entity {

public:
	float x_pos;
	float y_pos;
	int status;
	int HP;

};


bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{

	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;

}



//주인공 클래스 
class Hero :public entity {

public:
	void fire();
	void super_fire();
	void move(int i);
	void init(float x, float y);

	bool check_collision(float x, float y);
	bool bShow;
	bool show();
	void active();
	void hide();
};

bool Hero::check_collision(float x, float y)
{

	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		bShow = true;
		return true;

	}
	else {

		return false;
	}
}

bool Hero::show()
{
	return bShow;

}


void Hero::active()
{
	bShow = false;

}

void Hero::hide()
{
	bShow = true;

}


void Hero::init(float x, float y)
{

	x_pos = x;                                          
	y_pos = y;

}

void Hero::move(int i)
{
	switch (i)
	{
	case MOVE_UP:
		y_pos -= 3;
		break;

	case MOVE_DOWN:
		y_pos += 3;
		break;


	case MOVE_LEFT:
		x_pos -= 3;
		break;


	case MOVE_RIGHT:
		x_pos += 3;
		break;

	}

}




// 적 클래스 
class Enemy :public entity {

public:
	void fire();
	void init(float x, float y);
	void move();

};

void Enemy::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}


void Enemy::move()
{
	x_pos -= 2;
}


// 총알 클래스 
class Bullet :public entity {

public:
	bool bShow;

	void init(float x, float y);
	void move();
	void Bmove();
	bool show();
	void hide();
	void active();
	bool check_collision(float x, float y);

	bool Bcheck_collision(float x, float y);
};


bool Bullet::check_collision(float x, float y)
{

	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		bShow = false;
		return true;

	}
	else {

		return false;
	}
}


bool Bullet::Bcheck_collision(float x, float y)
{

	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		
		return true;

	}
	else {

		return false;
	}
}



void Bullet::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

}



bool Bullet::show()
{
	return bShow;

}


void Bullet::active()
{
	bShow = true;

}



void Bullet::move()
{
	x_pos += 8;
	//y_pos -= 8;
}

void Bullet::Bmove()
{
	x_pos -= 16;
}

void Bullet::hide()
{
	bShow = false;

}


class Block :public entity {

public:
	void fire();
	void init(float x, float y);
	void move();

	bool check_collision(float x, float y);
	bool bShow;
	bool show();
	void active();
	void hide();

};

bool Block::check_collision(float x, float y)
{

	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		int k = 0;
		
		if (k > 1){
			bShow = false;
			}
		return true;

	}
	else {

		return false;
	}
}

bool Block::show()
{
	return bShow;

}


void Block::active()
{
	bShow = false;

}

void Block::hide()
{
	bShow = true;

}
void Block::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}


void Block::move()
{
	x_pos -= 2;
}




//객체 생성 
Hero hero;
Enemy enemy[ENEMY_NUM];
Bullet bullet;
Block block[ENEMY_NUM];

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_EX_TOPMOST | WS_POPUP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);


	//게임 오브젝트 초기화 
	init_game();

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
			do_game_logic();

			render_frame();
		
		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);



		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Panel3.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"hero.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"enemy.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"bullet.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Block.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_block);    // load to sprite




	return;
}


void init_game(void)
{
	//객체 초기화 
	hero.init(0, 192);

	//적들 초기화 
	for (int k = 0;k < ENEMY_NUM;k++)
	{
		enemy[k].init(-64, 0);
	}

	//총알 초기화 
	bullet.init(hero.x_pos, hero.y_pos);

	//블럭 초기화
	for (int k = 0;k < ENEMY_NUM;k++)
	{
		block[k].init(-64, 0);
	}

	for (int k = 0;k < 3;k++)
	{
		for (int j = 1;j < 6;j++)
		{
			block[k * 5 + j].init((float)(64 * j + 64), (64 * k));
		}
	}
	
	for (int k = 3;k < 6;k++)
	{
		for (int j = 1;j < 6;j++)
		{
			block[k * 5 + j].init((float)(64 * j + 64), (64 * k+96));
		}
	}

	int j = 1;
	for (int k = 31;k < 50;k++)
	{
		block[k].init(64 * 6+j*64, 0);
			j++;
	}
	j = 1;
	for (int k = 50;k < 69;k++)
	{
		block[k].init(64 * 6 + j * 64, 64*7);
		j++;
	}

	j = 1;

	for (int k = 70;k < 78;k++)
	{
		block[k].init(64 * 9 + 64 * j, 64 * 4-30);
		j++;
	}
	j = 1;
	for (int k = 78;k < 86;k++)
	{
		block[k].init(64 * 9 + 64 * j, 64 * 3-30);
		j++;
	}
	
	block[87].init(64 * 16 , 64 * 1);
	block[88].init(64 * 16 , 64 * 2);
	block[89].init(64 * 17 , 64 * 1);
	block[90].init(64 * 17 , 64 * 2);

	block[91].init(64 * 11, 64 * 5-30);
	block[92].init(64 * 14, 64 * 6);
	block[93].init(64 * 17, 64 * 5-30);

	enemy[0].init(64 * 10, 64 * 5-30);
	enemy[1].init(64 * 11, 64 * 6);
	enemy[2].init(64 * 12, 64 * 5-30);
	enemy[3].init(64 * 13, 64 * 6);
	enemy[4].init(64 * 15, 64 * 6);
	enemy[5].init(64 * 17, 64 * 6);
	enemy[6].init(64 * 14, 64 * 5-30);
	enemy[7].init(64 * 16, 64 * 5-30);

	block[94].init(64 * 20, 64 * 5);
	block[95].init(64 * 20, 64 * 4);
	block[96].init(64 * 20, 64 * 3);
	block[100].init(64 * 20, 64 * 2+10);

	enemy[8].init(64 * 21, 64 * 1 );
	enemy[9].init(64 * 21, 64 * 5);
	enemy[10].init(64 * 21, 64 * 6);

	block[97].init(64 * 22, 64 * 6 );
	block[98].init(64 * 23, 64 * 6 );
	block[99].init(64 * 24, 64 * 6 );

	block[101].init(64 * 22, 64 * 1);
	block[102].init(64 * 23, 64 * 1);
	block[103].init(64 * 24, 64 * 1);

	block[104].init(64 * 23, 64 * 2);
	block[105].init(64 * 24, 64 * 2);
	block[106].init(64 * 23, 64 * 3);
	block[107].init(64 * 24, 64 * 3);


	enemy[11].init(64 * 24, 64 * 4);
	enemy[12].init(64 * 24, 64 * 5);
	
	enemy[13].init(64 * 25, 64 * 4);
	enemy[14].init(64 * 25, 64 * 5);
	
	enemy[15].init(64 * 26, 64 * 4);
	enemy[16].init(64 * 26, 64 * 5);


	block[108].init(64 * 25, 0);
	block[109].init(64 * 26, 0);
	block[110].init(64 * 27, 0);
	block[111].init(64 * 28, 0);
	block[112].init(64 * 29, 0);
	block[113].init(64 * 30, 0);
	block[114].init(64 * 31, 0);

	block[115].init(64 * 26, 64*7);
	block[116].init(64 * 27, 64*7);
	block[117].init(64 * 28, 64*7);
	block[118].init(64 * 25, 64*7);
	block[119].init(64 * 29, 64*7);
	block[120].init(64 * 30, 64*7);
	block[121].init(64 * 31, 64*7);

	block[122].init(64 * 28,64*5);
	block[123].init(64 * 29,64*5);
	block[124].init(64 * 30,64*5);
	block[125].init(64 * 28,64*2);
	block[126].init(64 * 29,64*2);
	block[127].init(64 * 30,64*2);
	block[128].init(64 * 28,64*3);
	block[129].init(64 * 29,64*3);
	block[130].init(64 * 30,64*3);
	block[131].init(64 * 28,64*4);
	block[132].init(64 * 29,64*4);
	block[133].init(64 * 30,64*4);


}


void do_game_logic(void)
{

	//주인공 처리 
	if (KEY_DOWN(VK_UP))
		hero.move(MOVE_UP);

	if (KEY_DOWN(VK_DOWN))
		hero.move(MOVE_DOWN);

	if (KEY_DOWN(VK_LEFT))
		hero.move(MOVE_LEFT);

	if (KEY_DOWN(VK_RIGHT))
		hero.move(MOVE_RIGHT);

	//

	//적들 처리 
	for (int i = 0; i < ENEMY_NUM; i++)
	{/*
		if (enemy[i].x_pos < 10)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 10; k++)
				{
					enemy[k + j * 10].init((float)(64 * k), (j * 64));
				}
			}

		}
		else
			enemy[i].move();
			*/

		enemy[i].move();
		block[i].move();
	}


	//총알 처리 
	if (bullet.show() == false)
	{
		if (hero.show() == false)
		{
			if (KEY_DOWN(VK_SPACE))
			{
				bullet.active();
				bullet.init(hero.x_pos, hero.y_pos);
			}
		}
	}


	if (bullet.show() == true)
	{
		if (bullet.x_pos > 800)
			bullet.hide();
		else
			bullet.move();


		//충돌 처리 
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			if (bullet.check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
			{
				// 쓰레기 위치로 옮기면 됨
				enemy[i].init((float)(-64), (0));
			}
			if (bullet.check_collision(block[i].x_pos, block[i].y_pos) == true)
			{
			}

		}
	}


	if (hero.show() == false)
	{
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			if (hero.check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
			{

			}
			if (hero.check_collision(block[i].x_pos, block[i].y_pos) == true)
			{

			}

		}
	}

	for (int k = 0; k < ENEMY_NUM;k++)
	{
		//충돌 처리 
			if (block[k].check_collision(bullet.x_pos, bullet.y_pos) == true)
			{

			}
		
	}
}



// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

											 //UI 창 렌더링 


											 /*
											 static int frame = 21;    // start the program on the final frame
											 if(KEY_DOWN(VK_SPACE)) frame=0;     // when the space key is pressed, start at frame 0
											 if(frame < 21) frame++;     // if we aren't on the last frame, go to the next frame

											 // calculate the x-position
											 int xpos = frame * 182 + 1;

											 RECT part;
											 SetRect(&part, xpos, 0, xpos + 181, 128);
											 D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
											 D3DXVECTOR3 position(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
											 d3dspt->Draw(sprite, &part, &center, &position, D3DCOLOR_ARGB(127, 255, 255, 255));
											 */

											 //주인공 
	if (hero.bShow == false)
	{
		RECT part;
		SetRect(&part, 0, 0, 64, 64);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	////총알 
	if (bullet.bShow == true)
	{
		RECT part1;
		SetRect(&part1, 0, 0, 64, 64);
		D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position1(bullet.x_pos, bullet.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_bullet, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
	}


	////에네미 // 블럭
	RECT part2;
	SetRect(&part2, 0, 0, 64, 64);
	D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

	for (int i = 0; i<ENEMY_NUM; i++)
	{
		D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth

		d3dspt->Draw(sprite_enemy, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	//// 에너미
	RECT part3;	
	SetRect(&part3, 0, 0, 64, 64);
	D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

	for (int i = 0; i<ENEMY_NUM; i++)
	{
		if (block[i].bShow == true) {
			D3DXVECTOR3 position3(block[i].x_pos, block[i].y_pos, 0.0f);    // position at 50, 50 with no depth

			d3dspt->Draw(sprite_block, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
		else
		{
			D3DXVECTOR3 position3(block[i].x_pos, block[i].y_pos, 0.0f);    // position at 50, 50 with no depth

			d3dspt->Draw(sprite_block, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));

		}
	}




	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}



// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero->Release();
	sprite_enemy->Release();
	sprite_bullet->Release();
	sprite_block->Release();

	return;
}
