//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include <map>
#include <unordered_map>
//---------------------------------------------------------------------------
#include <shared_mutex>
//---------------------------------------------------------------------------


class t_database
{
  public:
    t_database();

    ~t_database();

    // вставляем запись в таблицу
    std::string insert(const std::string& astr_table_name, int an_key, const std::string& astr_value);

    // удаляем всё из таблицы
    std::string truncate(const std::string& astr_table_name);

    // пересечение двух таблиц
    std::string intersection();

    // какая-то симметричная разница
    std::string symmetric_difference();

  private:

    // "набор" однотипных таблиц int, string
    std::unordered_map<std::string, std::map<int, std::string>> m_tables;


    // мьютекс синхронизации
    mutable std::shared_mutex m_mutex;


};
//---------------------------------------------------------------------------