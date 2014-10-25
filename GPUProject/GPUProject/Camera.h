#pragma once
#include "stdafx.h"

class Camera
{

public:
	Camera();
	~Camera();

	void		init(XMVECTOR p_pos, XMVECTOR p_up, XMVECTOR p_dir, float p_width, float p_height);
	void		update();
	void		setPosition(XMVECTOR p_pos);
	XMFLOAT4	getPosition();
	XMFLOAT4X4	getViewMat();
	XMFLOAT4X4	getProjMat();
	void		moveForeward();
	void		moveBackward();
	void		moveLeft();
	void		moveRight();
	void		moveUp();
	void		moveDown();
	void		moveDirection(float p_x, float p_y);
	XMFLOAT4    getUp();
private:
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;

	XMFLOAT4 m_pos;
	XMFLOAT4 m_up;
	XMFLOAT4 m_right;
	XMFLOAT4 m_look;
	XMFLOAT4 m_lookAt;
	float	 m_speed;
	float    m_mousePrevX;
	float    m_mousePrevY;
	float	 m_yaw;
	float	 m_pitch;

	void yaw(float p_radians);
	void pitch(float p_radians);

};