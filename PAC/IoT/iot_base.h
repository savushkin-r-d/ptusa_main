#pragma once

/*!
 * \brief �������������� ��� �������, ������������ ������
 */
#define SOCKID_ALTIVAR 1000

 /*!
  * \class i_iot_node
  *
  * \brief �����-��������� ��� ���������� ��������� � ��������� � ��������� ������������
  *
  */
class i_iot_node
	{
	public:
		virtual int set_state(int value, int index = 0);
		virtual int get_state(int index = 0);
		virtual int set_value(float value, int index = 0);
		virtual float get_value(int index = 0);
		virtual unsigned long set_counter(unsigned long value, int index = 0);
		virtual int get_counter(int index = 0);

		i_iot_node();
		virtual ~i_iot_node();
	};