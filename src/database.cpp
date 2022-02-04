//---------------------------------------------------------------------------
#include "database.h"
//---------------------------------------------------------------------------
#include "sync_console.h"
//---------------------------------------------------------------------------



t_database::t_database()
{
  m_tables.emplace("A", std::map<int, std::string>{});
  m_tables.emplace("B", std::map<int, std::string>{});
};
//---------------------------------------------------------------------------


t_database::~t_database()
{
}
//---------------------------------------------------------------------------


// вставляем запись в таблицу
std::string t_database::insert(const std::string& astr_table_name, int an_key, const std::string& astr_value)
{
  std::unique_lock lock(m_mutex);

  auto iter = m_tables.find(astr_table_name);
  if (iter == m_tables.end()) {
    clog::log_err("table not found: " + astr_table_name);
    return "ERR table not found: " + astr_table_name;
  }

  auto& map = (*iter).second;

  auto result = map.emplace(an_key, astr_value);
  if (!result.second) {
    clog::log_err("ERR element already exist: " + std::to_string(an_key));
    return "ERR element already exist: " + std::to_string(an_key);
  }

  return "OK";
}
//---------------------------------------------------------------------------


// удаляем всё из таблицы
std::string t_database::truncate(const std::string& astr_table_name)
{
  std::unique_lock lock(m_mutex);

  for(auto& map : m_tables) {
    map.second.clear();
  }

  return "OK";
}
//---------------------------------------------------------------------------


// пересечение двух таблиц
std::string t_database::intersection()
{
  std::shared_lock lock(m_mutex);

  auto& table_a = (m_tables.find("A"))->second;
  auto& table_b = (m_tables.find("B"))->second;


  std::string str_result;

  for(auto& curr : table_a)
  {
    auto iter = table_b.find(curr.first);
    if (iter == table_b.end()) {
      continue;
    }

    if(str_result.size()) {
      str_result += "\r\n";
    }
    str_result += std::to_string(curr.first) + ", " + curr.second + ", " + iter->second;
  }

  return str_result + "\r\nOK";
}
//---------------------------------------------------------------------------


// какая-то симметричная разница
std::string t_database::symmetric_difference()
{
  std::shared_lock lock(m_mutex);

  auto& table_a = (m_tables.find("A"))->second;
  auto& table_b = (m_tables.find("B"))->second;


  std::string str_result;

  for(auto& curr : table_a)
  {
    auto iter = table_b.find(curr.first);
    if (iter != table_b.end()) {
      continue;
    }

    if (str_result.size()) {
      str_result += "\r\n";
    }
    str_result += std::to_string(curr.first) + ", " + curr.second + ", ";
  }


  for (auto& curr : table_b)
  {
    auto iter = table_a.find(curr.first);
    if (iter != table_a.end()) {
      continue;
    }

    if (str_result.size()) {
      str_result += "\r\n";
    }
    str_result += std::to_string(curr.first) + ", , " + curr.second;
  }

  return str_result + "\r\nOK";
}
//---------------------------------------------------------------------------