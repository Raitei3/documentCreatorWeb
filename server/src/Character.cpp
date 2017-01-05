#include "../headers/Character.hpp"


Character::Character():m_baseline(0)
{}

Character::Character(const std::string &l):
  m_label(l), m_baseline(0)
{}


std::string Character::getLabel() const
{
  return m_label;
}

void Character::setBaseline(int x)
{
  m_baseline = x;
}

int Character::getBaseline() const
{
  return m_baseline;
}

int Character::countComponent() const
{
  return m_idComponentsConnexes.size();
}
void Character::addComponent(int idCC, int idLine)
{
  if(!hasComponent(idCC, idLine))
    m_idComponentsConnexes.push_back(std::make_pair(idLine,idCC));
}

void Character::removeComponent(int idCC, int idLine)
{
  if(hasComponent(idCC, idLine))
    m_idComponentsConnexes.erase(std::remove(m_idComponentsConnexes.begin(), m_idComponentsConnexes.end(), std::make_pair(idLine,idCC)), m_idComponentsConnexes.end()); 
}

bool Character::hasComponent(int idCC, int idLine) const
{
  for (const auto &s : m_idComponentsConnexes) {
    if(s.first == idLine && s.second == idCC)
      return true;
  }
  return false;

}

std::pair<int,int> Character::getIdComponentAtIndex(int index) const
{
  assert(index>=0 && size_t(index)<=m_idComponentsConnexes.size());
  return m_idComponentsConnexes.at(index);
}

bool operator== ( const Character &c1, const Character &c2) 
{
  return c1.m_label == c2.m_label;
}
