#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::init(XMVECTOR p_pos, XMVECTOR p_up, XMVECTOR p_dir, float p_width, float p_height)
{
	XMStoreFloat4(&m_pos, p_pos);
	XMStoreFloat4(&m_up, p_up);
	XMStoreFloat4(&m_look, p_dir);
	XMVECTOR vRight = XMVector3Cross(p_up, p_dir);
	XMStoreFloat4(&m_right, vRight);
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(0.4f*3.14f, p_width / p_height, 1.f, 500.f);
	XMStoreFloat4x4(&m_proj, mProj);
	m_speed = 1.f;
}

void Camera::update()
{
	XMVECTOR vLook = XMLoadFloat4(&m_look);
	XMVECTOR vUp = XMLoadFloat4(&m_up);
	XMVECTOR vPos = XMLoadFloat4(&m_pos);
	XMVECTOR vLookAt = vLook + vPos;
	XMStoreFloat4(&m_lookAt, vLookAt);

	XMMATRIX mView = XMMatrixLookAtLH(vPos, vLookAt, vUp);
	XMStoreFloat4x4(&m_view, mView);
}

void Camera::setPosition(XMVECTOR p_pos)
{
	XMStoreFloat4(&m_pos, p_pos);
}

XMFLOAT4 Camera::getPosition()
{
	return m_pos;
}

XMFLOAT4 Camera::getUp()
{
	return m_up;
}

XMFLOAT4X4 Camera::getProjMat()
{
	return m_proj;
}

XMFLOAT4X4 Camera::getViewMat()
{
	return m_view;
}

void Camera::moveForeward()
{
	XMVECTOR vPos = XMLoadFloat4(&m_pos);
	XMVECTOR vLook = XMLoadFloat4(&m_look);
	vPos += vLook * m_speed;
	XMStoreFloat4(&m_pos, vPos);
}

void Camera::moveBackward()
{
	XMVECTOR vPos = XMLoadFloat4(&m_pos);
	XMVECTOR vLook = XMLoadFloat4(&m_look);
	vPos += vLook * -m_speed;
	XMStoreFloat4(&m_pos, vPos);
}

void Camera::moveLeft()
{
	XMVECTOR vPos = XMLoadFloat4(&m_pos);
	XMVECTOR vRight = XMLoadFloat4(&m_right);
	vPos += vRight * -m_speed;
	XMStoreFloat4(&m_pos, vPos);
}

void Camera::moveRight()
{
	XMVECTOR vPos = XMLoadFloat4(&m_pos);
	XMVECTOR vRight = XMLoadFloat4(&m_right);
	vPos += vRight * m_speed;
	XMStoreFloat4(&m_pos, vPos);
}

void Camera::moveUp()
{
	m_pos.y += m_up.y * m_speed;
}

void Camera::moveDown()
{
	m_pos.y -= m_up.y * m_speed;
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

	//Do the transform.
	XMVECTOR vUp = XMLoadFloat4(&m_up);
	XMMATRIX mat= XMMatrixRotationAxis(vUp, -radians);
	XMVECTOR vRight = XMLoadFloat4(&m_right);
	XMVECTOR vLook = XMLoadFloat4(&m_look);
	vRight = XMVector3TransformNormal(vRight, mat);
	vLook = XMVector3TransformNormal(vLook, mat);
	XMStoreFloat4(&m_right, vRight);
	XMStoreFloat4(&m_look, vLook);
}

void Camera::pitch(float radians)
{
	//if the angle hasn't changed, return.
    if (radians == 0.0f)
    {
        return;
    }
	//Do the transfrom.
	XMVECTOR vRight = XMLoadFloat4(&m_right);
	XMMATRIX mat = XMMatrixRotationAxis(vRight, radians);
	XMVECTOR vLook = XMLoadFloat4(&m_look);
	XMVECTOR vLookAt = XMLoadFloat4(&m_lookAt);
	XMVECTOR vUp = XMLoadFloat4(&m_up);
	vLook = XMVector3TransformNormal(vLook, mat);
	vLookAt = XMVector3TransformNormal(vUp, mat);
	XMStoreFloat4(&m_look, vLook);
	XMStoreFloat4(&m_lookAt, vLookAt);
}