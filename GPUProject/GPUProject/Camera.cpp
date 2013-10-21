#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::init(XMVECTOR p_pos, XMVECTOR p_up, XMVECTOR p_dir, float p_width, float p_height)
{
	m_pos	= p_pos;
	m_up	= p_up;
	
	m_look	= p_dir;

	m_right = XMVector3Cross(m_up, m_look);
	m_proj = XMMatrixPerspectiveFovLH(0.4f*3.14, p_width / p_height, 1.0f, 500.0f);
	m_speed = 1.f;
}

void Camera::update()
{
	//m_right = XMVector3Cross(m_up, m_look);
	m_lookAt = m_look + m_pos;
	//m_lookAt =XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_view = XMMatrixLookAtLH(m_pos, m_lookAt, m_up);
}

void Camera::setPosition(XMFLOAT3 p_pos)
{
	m_pos = XMLoadFloat3(&p_pos);
}

XMVECTOR Camera::getPosition()
{
	return m_pos;
}

XMVECTOR Camera::getUp()
{
	return m_up;
}

XMMATRIX Camera::getProjMat()
{
	return m_proj;
}

XMMATRIX Camera::getViewMat()
{
	return m_view;
}

void Camera::moveForeward()
{
	m_pos += m_look * m_speed;
}

void Camera::moveBackward()
{
	m_pos += m_look * -m_speed;
}

void Camera::moveLeft()
{
	m_pos += m_right * -m_speed;
}

void Camera::moveRight()
{
	m_pos += m_right * m_speed;
}

void Camera::moveDirection(float p_x, float p_y)
{
	if(p_x != m_mousePrevX || p_y != m_mousePrevY)
	{
		yaw(p_x*0.001f);
		pitch(p_y*0.001f);
		m_mousePrevX = p_x;
		m_mousePrevY = p_y;
	}
}

void Camera::yaw(float radians)
{
	//if the angle hasn't changed, return.
    if (radians == 0.0f)
    {
        return;
    }

	//Do the transfrom.
	XMMATRIX mat;
	mat		= XMMatrixRotationAxis(m_up, -radians);
	m_right = XMVector3TransformNormal(m_right, mat);
	m_look	= XMVector3TransformNormal(m_look, mat);
}

void Camera::pitch(float radians)
{
	//if the angle hasn't changed, return.
    if (radians == 0.0f)
    {
        return;
    }
	//Do the transfrom.
	XMMATRIX mat;
	mat	= XMMatrixRotationAxis(m_right, radians);
	m_look	= XMVector3TransformNormal(m_look, mat);
	m_lookAt= XMVector3TransformNormal(m_up, mat);
}