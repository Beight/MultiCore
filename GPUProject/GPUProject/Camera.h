#ifndef _CAMERA__H
#define _CAMERA__H

#include "stdafx.h"



class Camera
{

public:
	Camera();
	~Camera();

	void		init(XMVECTOR p_pos, XMVECTOR p_up, XMVECTOR p_dir, float p_width, float p_height);
	void		update();
	void		setPosition(XMFLOAT3 p_pos);
	XMVECTOR	getPosition();
	XMMATRIX	getViewMat();
	XMMATRIX	getProjMat();
	void		moveForeward();
	void		moveBackward();
	void		moveLeft();
	void		moveRight();
	void		moveUp();
	void		moveDown();
	void		moveDirection(float p_x, float p_y);
	XMVECTOR    getUp();
private:
	XMMATRIX m_view;
	XMMATRIX m_proj;

	XMVECTOR m_pos;
	XMVECTOR m_up;
	XMVECTOR m_right;
	XMVECTOR m_look;
	XMVECTOR m_lookAt;
	float	 m_speed;
	float    m_mousePrevX;
	float    m_mousePrevY;
	float	 m_yaw;
	float	 m_pitch;

	void yaw(float p_radians);
	void pitch(float p_radians);

};

#endif