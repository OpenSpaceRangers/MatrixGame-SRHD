#pragma once

enum ButtonStatus
{
    B_DOWN,
    B_UP,
    B_DOUBLE,

    B_WHEEL
};

enum ESysEvent
{
    SYSEV_DEACTIVATING,
    SYSEV_ACTIVATED,
};

class CForm : public CMain {
	private:
		CForm * m_FormPrev;
		CForm * m_FormNext;

	protected:
		CWStr m_Name;		// ����� ������ �������� ��� ��� �������� �������

	public:

        static void StaticInit(void);

		CForm(void);
		~CForm();

		virtual void Enter(void)=0;
		virtual void Leave(void)=0;
		virtual void Draw(void)=0;

		virtual void Takt(int)=0;

		virtual void MouseMove(int,int)=0;
		virtual void MouseKey(ButtonStatus,int,int,int) = 0;

        virtual void Keyboard(bool, int) = 0;

        virtual void SystemEvent(ESysEvent se)=0;
};

extern CForm * g_FormFirst;
extern CForm * g_FormLast;
extern CForm * g_FormCur;

void FormChange(CForm * form);
