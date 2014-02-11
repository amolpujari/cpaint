#include <graphics.h>
#include <dos.h>
#include <dir.h>
#include<conio.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define valid(x) (x>='a'&&x<='z'||x>='A'&&x<='Z'||x>='0'&&x<='9'||x==')'||x=='('||x==13||x==8||x==' '||x=='*'||x=='%'||x=='('||x==')'||x==','||x=='.'||x=='/'||x=='\t')

static int gx,gy,bufi;
static char buf[47][106];
static int ti;
static char t[106],oldpath[8];

void finish()
{
	chdir(oldpath);
	closegraph();
	clrscr();
	exit(0);
}
void initg()
{
	int gdriver = DETECT, gmode, errorcode;
	initgraph(&gdriver, &gmode, "");
	settextstyle(2,0,4);
	setcolor(3);
	setpalette(3,43);//text
	setpalette(13,198);//cursor
	setpalette(4,294);//error
}
void beep()
{
	sound(100);
	delay(20);
	nosound();
}
void scroll()
{
	setfillstyle(0,0);
	bar(0,0,640,480);
	for(int i=bufi,j=0;i<47;i++)
	{
		if(buf[i][0]=='ú')setcolor(4);
		outtextxy(0,j++*10,buf[i]);
		setcolor(3);
	}
	for(i=0;i<bufi;i++)
	{
		if(buf[i][0]=='ú')setcolor(4);
		outtextxy(0,j++*10,buf[i]);
		setcolor(3);
	}
	gy=47;
}
int prnfl(char fl[])
{
	FILE *fp;
	if(fl==0) strcpy(fl,"out.db");
	if((fp=fopen(fl,"r"))==0) return 1;
	int k=0;
	while(fgets(buf[bufi],105,fp)!=0)
	{
		if(gy!=48)outtextxy(0,gy++*10,buf[bufi]);
		else k=1;
		bufi++;
		if(bufi==47) bufi=0;
	}
	if(k)	scroll();
	gx=-1;
	fclose(fp);
	return 0;
}
void prnstr(char t[])
{
	strcpy(buf[bufi++],t);
	if(bufi==47)bufi=0;
	if(t[0]=='ú')setcolor(4);
	outtextxy(gx*6,gy*10,t);
	setcolor(3);
	gy++;gx=0;
}
void msg(char s[],int fe=0)
{
	prnstr("  ");
	if(fe) prnstr("ú E R R O R");
	prnstr(s);
	prnstr("   ");
	gx--;
}
void setcmd()
{
	if(!stricmp("cl",t))
	{
		setfillstyle(0,0);
		bar(0,0,640,480);
		gx=-1,gy=0;
		for(int i=0;i<47;i++) buf[i][0]=0;
		bufi=0;
		return;
	}
	if(!stricmp("x",t)) finish();
	if(!strnicmp("/",t,1)) prnfl(t+1);
}
void prn(char s[])
{
	if( s[0]==8 )
	{
		if(gx)
		{
			t[--ti]=0;
			gx--;
			setfillstyle(0,0);
			bar(gx*6, gy*10+2,(gx+1)*6, (gy+2)*10);
			return;
		}
		beep();
		ti=0;
		return;
	}
	if(s[0]=='\t')
	{
		for(int i=0;i<8;i++)
		prn(" ");
	}
	if(s[0]==13)
	{
		gy++,gx=-1;
		t[ti]=0;
		strcpy(buf[bufi++],t);
		buf[bufi-1][ti]=0;
		if(bufi==47)bufi=0;
		setcmd();
		ti=0;
	}
	else	outtextxy(gx*6,gy*10,s),t[ti++]=s[0];
	gx++;
	if(gx>=105)
	{
		gy++;gx=0;
		t[ti]=0;
		strcpy(buf[bufi++],t);
		if(bufi==47)bufi=0;
		ti=0;
	}
	if(gy>=48) scroll();
}
void in()
{
	char s[1],ch;

	s[1]=0;
	setfillstyle(1,13);
	bar(gx*6, gy*10+2,(gx+1)*6-3, (gy+1)*10-1);

	while(1)
	{
		ch=getch();
		if(! valid(ch) )
		{
			beep();
			continue;
		}
		s[0]=ch;
		setfillstyle(0,0);
		bar(gx*6, gy*10+2,(gx+1)*6, (gy+2)*10);
		prn(s);
		setfillstyle(1,13);
		bar(gx*6, gy*10+2,(gx+1)*6-3, (gy+1)*10-1);
	}
}
char* getstr(char in[],int len=100)
{
	char s[1],ch;
	int n=strlen(in);

	if(bufi==47) bufi=0;
	if(gy>=48) scroll();

	gx=0;
	strcpy(buf[bufi],in);
	ti=n;
	outtextxy(gx*6,gy*10,buf[bufi]);
	gx=n;
	s[1]=0;

	setfillstyle(1,13);
	bar(gx*6, gy*10+2,(gx+1)*6-3, (gy+1)*10-1);

	while( (ch=getch())!=13)
	{
		if( ch==8 && gx<=n || (!valid(ch)) || ti>=(len+n) && ch!=8 )
		{
			beep();
			continue;
		}
		s[0]=ch;
		setfillstyle(0,0);
		bar(gx*6, gy*10+2,(gx+1)*6, (gy+2)*10);
		prn(s);
		setfillstyle(1,13);
		bar(gx*6, gy*10+2,(gx+1)*6-3, (gy+1)*10-1);
	}
	setfillstyle(0,0);
	bar(gx*6, gy*10+2,(gx+1)*6, (gy+2)*10);

	char *t1;
	t1=new char[ti-n+1];
	for(int i=0;i<len&&t;i++)
	t1[i]=t[i+n];
	t1[ti-n]=0;

	strcat(buf[bufi],t1);

	bufi++;
	if(bufi==47) bufi=0;
	gy++;
	if(gy>=48) scroll();
	return t1;
}
void main()
{
	initg();
	prnfl("matrixR.txt");
	in();
}