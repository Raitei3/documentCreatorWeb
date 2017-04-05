#include "Font.hpp"


Font::Font()
{
}

void Font::setName(const std::string &n)
{
	m_name = n;
}

std::string Font::getName() const
{
	return m_name;
}

void Font::addCharacter(const Character &c)
{
  m_listCharacter.push_back(c);
}

int Font::indexOfCharacter(const std::string &c)
{
  for (unsigned i=0; i<m_listCharacter.size(); i++)
  {
    const Character &s = m_listCharacter.at(i);
    if(s.getLabel() == c)
      return i;
  }
  return -1;
}

int Font::indexOfCharacterForCC(int idCC, int idLine)
{
  for (const auto &s : m_listCharacter)
    if(s.hasComponent(idCC, idLine))
      return find(m_listCharacter.begin(), m_listCharacter.end(), s) - m_listCharacter.begin();;
  return -1;
}

Character* Font::characterAtIndex(int id)
{
  assert(id>=0 && id<countCharacter());
  return &m_listCharacter.at(id);
}

const Character* Font::characterAtIndex(int id) const
{
  assert(id>=0 && id<countCharacter());
  return &m_listCharacter.at(id);
}

void Font::removeCharacter(int id)
{
  assert(id>=0 && id<countCharacter());
  m_listCharacter.erase(m_listCharacter.begin() + id);
}

int Font::countCharacter() const
{
  return m_listCharacter.size();
}
