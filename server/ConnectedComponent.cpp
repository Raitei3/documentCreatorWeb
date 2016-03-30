#include "ConnectedComponent.hpp"

ConnectedComponent::ConnectedComponent():m_baseline(0)
{
	m_up = 0;
	m_down = 0;
	m_left = 0;
	m_right = 0;
}

ConnectedComponent::ConnectedComponent(std::vector<cv::Point> vec):m_listPoint(vec), m_baseline(0){
	m_up = 0;
	m_down = 0;
	m_left = 0;
	m_right = 0;
}

const std::vector<cv::Point> ConnectedComponent::getListPoint() const
{
	return m_listPoint;
}

void ConnectedComponent::setBaseline(int x)
{
	m_baseline = x;
}

const int ConnectedComponent::getBaseline() const
{
	return m_baseline;
}

const bool ConnectedComponent::getInline() const
{
	return m_inline;
}

void ConnectedComponent::setInline(bool x)
{
	m_inline = x;
}

void ConnectedComponent::setBoundingBox(int up, int down, int left, int right){
	m_up = up;
	if(m_down == 0)
		m_baseline = down;
	m_down = down;
	m_right = right;
	m_left = left;
}
cv::Rect ConnectedComponent::getBoundingBox(){
	if (m_up == 0 && m_down == 0 && m_left == 0 && m_right == 0)
	{
		cv::Rect bb = cv::boundingRect(m_listPoint);
		m_up = bb.y;
		m_down = bb.y + bb.height;
		m_left = bb.x;
		m_right = bb.x + bb.width;
	}
	return cv::Rect(m_left, m_up, m_right - m_left, m_down - m_up); 
}
