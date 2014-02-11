#include<stdio.h>
#include<graphics.h>

struct BYTEREGS
{
	unsigned char  al, ah, bl, bh;
	unsigned char  cl, ch, dl, dh;
};
struct WORDREGS
{
	unsigned int  ax, bx, cx, dx;
	unsigned int  si, di, cflag, flags;
};
union REGS
{
	struct  WORDREGS  x;
	struct  BYTEREGS  h;
}i,o;
int gd=DETECT,gm,maxx,maxy,x,y,button,a,b,j,userpat=1,pro;
char flag=0,str[8],xycol=-1,fillstyle=1,p=1,bcol=0,filename[11]="new.pix";
char buf[100][200],linestyle=0,l=0,thickness=1,color=15,mode=0,m=0;
char pattern[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
FILE *fp;

int initmouse()
{
	i.x.ax=0;
	int86(0x33,&i,&o);
	return(o.x.ax);
}

void showmouse()
{
	i.x.ax=1;
	int86(0x33,&i,&o);
}

void hidemouse()
{
	i.x.ax=2;
	int86(0x33,&i,&o);
}

void restrictmouse(int x1,int y1,int x2,int y2)
{
	i.x.ax=7;
	i.x.cx=x1;
	i.x.dx=x2;
	int86(0x33,&i,&o);
	i.x.ax=8;
	i.x.cx=y1;
	i.x.dx=y2;
	int86(0x33,&i,&o);
}

void getmousepos(int *butoon,int *x, int *y)
{
	i.x.ax=3;
	int86(0x33,&i,&o);
	*butoon=o.x.bx;
	*x=o.x.cx;
	*y=o.x.dx;
}

void putpix(int col,int row,int color)
{
	i.h.ah=0x0c;
	i.h.al=color;
	i.x.cx=col;
	i.x.dx=row;
	i.h.bh=0;
	int86(0x010,&i,&o);
}
void to_buf(int a,int b,int ax,int by)
{
	int i,j;

	for(j=0;j<by;j++)
	for(i=0;i<ax;i++)
	{
		buf[i][j]  = getpixel( a+(i*2)   , b+j ) << 4;
		buf[i][j] |= getpixel( a+(i*2)+1 , b+j );
	}
}

void from_buf(int a,int b,int ax,int by)
{
	int i,j;

	for(j=0;j<by;j++)
	for(i=0;i<ax;i++)
	{
		putpix( a+(i*2)   , b+j , ( buf[i][j] & 0xf0 ) >> 4 );
		putpix( a+(i*2)+1 , b+j ,   buf[i][j] & 0x0f        );
	}
}
void fillblock(int a,int b,int c,int d,int style,int colr)
{
	setfillstyle(style,colr);
	hidemouse();
	bar(a,b,c,d);
	showmouse();
}

void show_box(char *msg)
{
	fillblock(225,175,424,234,1,8);
	setcolor(7);
	rectangle(225,175,424,193);
	rectangle(225,193,424,234);
	setcolor(15);
	settextstyle(2,0,4);
	outtextxy(230,179,msg);
}
void highlightfillstyle()
{
	int a=0,b=0,c,d;

	setcolor(7);
	rectangle(4,219,61,293);

	for(j=1;j<13;j++)
	{
		if(fillstyle==j) c=a,d=b;
		fillblock(5+b,220+a,23+b,238+a,j,7);

		b+=18;

		if(j%3==0) a+=18,b=0;
	}

	if(fillstyle!=12)
	{
		fillblock(5+d,220+c,23+d,238+c,fillstyle,14);
		setfillpattern(pattern,7);
	}
	else
	setfillpattern(pattern,14);

	bar(41,276,59,292);
}
void highlightlinestyle()
{
	int a=0,b=0,c,d;

	setcolor(7);
	setlinestyle(0,0,0);
	gotoxy(7,16);
	printf("%d",thickness);
	rectangle(4,219,61,255);
	line(23,219,23,255);
	line(42,219,42,255);
	line(5,237,60,237);

	for(j=0;j<5;j++)
	{
		if(linestyle==j) c=a,d=b;
		setcolor(1);
		setlinestyle(0,userpat,3);
		line(8+b,229+a,20+b,229+a);
		setcolor(7);
		setlinestyle(j,userpat,thickness);
		line(8+b,229+a,20+b,229+a);
		b+=18;
		if((j+1)%3==0) a+=18,b=0;
	}

	setcolor(14);
	setlinestyle(linestyle,userpat,thickness);
	line(8+d,229+c,20+d,229+c);
}
void highlightmode()
{

	hidemouse();
	fillblock(4,219,61,294,1,1);
	setlinestyle(0,0,0);
	setcolor(8);

	for(j=19;j<180;j=j+32)
	{
		rectangle( 0, j,30, j+30);
		rectangle(32, j,62, j+30);
	}

	j=(mode/2)*32+19;

	setcolor(15);

	if(mode%2)
	rectangle(32,j,62,j+30);
	else
	rectangle(0,j,30,j+30);

	if(mode==6||mode==7)
	highlightfillstyle() ;

	if(mode==4||mode==8)
	highlightlinestyle() ;

	showmouse();
}
#define dotsmacro if( z || p<(maxx-10) && p>70 && q<(maxy-30) && q>20 ) putpix(p,q,color)

void dots(int z)
{
	int a,b,c,d,p,q,u;

	j=(x+y+13)%6;

	switch(j)
	{
	case 0 :  a=1,b=2,c=3,d=4; break;
	case 1 :  a=2,b=3,c=4,d=1; break;
	case 2 :  a=3,b=4,c=1,d=2; break;
	case 3 :  a=4,b=1,c=2,d=3; break;
	case 4 :  a=5,b=4,c=2,d=3; break;
	case 5 :  a=0,b=2,c=1,d=4; break;
	}

	if(z) u=color,color=7;

	p=x+a,q=y+d; dotsmacro;
	p=x+b,q=y-c; dotsmacro;
	p=x-c,q=y+b; dotsmacro;
	p=x-d,q=y-a; dotsmacro;

	p=x+a,q=y;   dotsmacro;
	p=x-b,q=y;   dotsmacro;
	p=x+c,q=y;   dotsmacro;
	p=x-d,q=y;   dotsmacro;

	p=x,q=y+a;   dotsmacro;
	p=x,q=y-b;   dotsmacro;
	p=x,q=y+c;   dotsmacro;
	p=x,q=y-d;   dotsmacro;

	p=x+a,q=y+d; dotsmacro;
	p=x+b,q=y-c; dotsmacro;
	p=x-c,q=y+b; dotsmacro;
	p=x-d,q=y-a; dotsmacro;

	if(z) color=u,setcolor(u);
}

void bigdots(int z)
{
	int a,b,c,d,e,f,g,p,q,u;

	j=(x+y+21)%11;

	switch(j)
	{
	case  0 :  a=1,b=2,c=3,d=4,e=5,f=6,g=7; break;
	case  2 :  a=2,b=3,c=4,d=5,e=6,f=7,g=1; break;
	case  3 :  a=3,b=4,c=5,d=6,e=7,f=1,g=2; break;
	case  4 :  a=4,b=5,c=6,d=7,e=1,f=2,g=3; break;
	case  5 :  a=5,b=6,c=7,d=1,e=2,f=3,g=4; break;
	case  6 :  a=6,b=7,c=1,d=2,e=3,f=4,g=5; break;
	case  7 :  a=7,b=1,c=2,d=3,e=4,f=5,g=6; break;
	case  8 :  a=8,b=6,c=5,d=4,e=3,f=7,g=2; break;
	case  9 :  a=9,b=2,c=1,d=6,e=3,f=7,g=5; break;
	case 10 :  a=0,b=3,c=6,d=4,e=5,f=2,g=1; break;
	}

	if(z) u=color,color=7;

	p=x+a,q=y+d; dotsmacro;
	p=x+b,q=y-e; dotsmacro;
	p=x-c,q=y+f; dotsmacro;
	p=x-d,q=y-g; dotsmacro;

	p=x+a,q=y;   dotsmacro;
	p=x-c,q=y;   dotsmacro;
	p=x+e,q=y;   dotsmacro;
	p=x-f,q=y;   dotsmacro;

	p=x,q=y+b;   dotsmacro;
	p=x,q=y-d;   dotsmacro;
	p=x,q=y+f;   dotsmacro;
	p=x,q=y-g;   dotsmacro;

	p=x+a,q=y+b; dotsmacro;
	p=x+d,q=y-c; dotsmacro;
	p=x-e,q=y+f; dotsmacro;
	p=x-a,q=y-g; dotsmacro;

	p=x+d,q=y+d; dotsmacro;
	p=x+c,q=y-e; dotsmacro;
	p=x-b,q=y+f; dotsmacro;
	p=x-a,q=y-g; dotsmacro;

	p=x+b,q=y;   dotsmacro;
	p=x-d,q=y;   dotsmacro;
	p=x+e,q=y;   dotsmacro;
	p=x-f,q=y;   dotsmacro;

	p=x,q=y+b;   dotsmacro;
	p=x,q=y-a;   dotsmacro;
	p=x,q=y+c;   dotsmacro;
	p=x,q=y-g;   dotsmacro;

	p=x+a,q=y+g; dotsmacro;
	p=x+f,q=y-e; dotsmacro;
	p=x-c,q=y+d; dotsmacro;
	p=x-b,q=y-a; dotsmacro;

	if(z) color=u,setcolor(u);
}
void colrblock(int c)
{
	color=c;
	fillblock(19,412,51,444,1,color);
	setcolor(color);
}

void setscreen(int q)
{
	int j,k,l,poly[12];

	if(q==0)
	{
		initgraph(&gd,&gm,"d:\\tc3\\bgi");
		if(graphresult()!=grOk) exit(1);
		maxx=getmaxx();
		maxy=getmaxy();
	}

	setlinestyle(0,0,0);
	setcolor(1);

	poly[ 0] = 0;	        poly[ 1] = 0;
	poly[ 2] = maxx+1;      poly[ 3] = 0;
	poly[ 4] = maxx+1;      poly[ 5] = maxy+1;
	poly[ 6] = maxx-10;     poly[ 7] = maxy+1;
	poly[ 8] = maxx-10;     poly[ 9] = 21;
	poly[10] = 0;           poly[11] = 21;

	setfillstyle(1,1);
	fillpoly(6, poly);

	poly[ 0] = 0;           poly[ 1] = 21;
	poly[ 2] = 70;          poly[ 3] = 21;
	poly[ 4] = 70;          poly[ 5] = maxy-30;
	poly[ 6] = maxx-10;     poly[ 7] = maxy-30;
	poly[ 8] = maxx-10;     poly[ 9] = maxy+1;
	poly[10] = 0;           poly[11] = maxy+1;

	fillpoly(6, poly);

	setcolor(8);
	rectangle(69,19,maxx-9,maxy-29);
	rectangle(70,20,maxx-10,maxy-30);
	rectangle(3,364,67,381);
	rectangle(3,383,67,398);
	highlightmode(0);
	setlinestyle(0,0,0);
	setcolor(7);
	rectangle(17,410,52,445);
	line(12,31,15,31);
	line(12,32,15,32);
	line(12,33,15,33);
	line(12,34,15,34);
	line(50,25,40,42);
	line(51,26,43,43);
	line(52,27,41,44);
	putpix(40,45,7);
	putpix(40,43,7);
	rectangle(5,88,23,102);
	rectangle(10,90,27,110);
	ellipse(45,96,0,360,10,10);
	ellipse(50,106,0,360,10,4);
	setfillstyle(1,7);
	rectangle(5,120,23,134);
	floodfill(6,121,7);
	rectangle(10,122,27,142);
	ellipse(45,128,0,360,10,10);
	ellipse(50,138,0,360,10,4);
	floodfill(45,128,7);
	floodfill(50,138,7);
	line(7,156,20,169);
	rectangle(6,155,8,157);
	rectangle(19,168,21,170);
	line(47,202,58,198);//color pot
	line(46,201,43,193);
	line(59,197,56,189);
	putpix(41,192,7);
	line(44,192,55,188);
	ellipse(41,198,0,360,1,5);
	ellipse(41,205,0,360,6,1);
	line(38,202,42,202);
	line(38,203,44,203);
	line(37,205,47,205);
	line(41,194,41,205);

	settextstyle(1,0,3);
	outtextxy(34,142,"A");
	settextstyle(1,0,2);
	outtextxy(49,150,"a");
	rectangle(5,200,15,205);
	line(5,200,15,187);
	line(15,200,25,187);
	line(15,187,25,187);
	line(25,187,25,190);
	line(25,190,15,205);
	y=65,x=15;
	dots(1);
	x=46;
	bigdots(1);

	setcolor(7);
	settextstyle(2,0,4);

	gotoxy(10,1);	printf("%-11s",filename);

	outtextxy(343,2,"Image");
	outtextxy(398,2,"File");
	outtextxy(445,2,"Blackend");
	outtextxy(534,2,"Clear");
	outtextxy(597,2,"Exit");



	setcolor(8);
	line(245,458,maxx-40,458);
	line(246,459,maxx-39,459);

	setcolor(8);
	rectangle(18,411,51,444);

	for(k=0,j=250;k<16;k++)
	{
	      setcolor(8);
	      line(j-3,460,j+17,480);
	      line(j-2,460,j+18,480);
	      line(j-1,460,j+19,480);
	      setcolor(k);

	      for(l=0;l<20;l++)
	      line(j+l,460,j+l+20,480);
	      j=j+22;
	}

	setcolor(8);
	line(j-3,460,j+17,480);
	line(j-2,460,j+18,480);
	line(j-1,460,j+19,480);

	setcolor(14);
	settextstyle(3,0,1);
	outtextxy(8,450,"Drawpix");

	if(!q) colrblock(7);
	else   colrblock(color);

	setlinestyle(linestyle,userpat,thickness);
}
void rect()
{
	static int c=-1,d=-1;

	if( c==-1 && (button & 1)==1 ) c=x,d=y;
	if( c!=-1 && (button & 1)==1 ) putpix(c,d,color);
	if( c!=-1 && (button & 1)==0 )
	{
		hidemouse();
		rectangle(c,d,x,y);
		if(mode==6 )
		{
			if(fillstyle!=12)
			setfillstyle(fillstyle,color);
			else
			setfillpattern(pattern,color);

			floodfill((c+x)/2,(d+y)/2,color);
		}
		showmouse();
		c=d=-1;
	}
}

void cir()
{
	static int c=-1,d=-1;
	int rx=c-x,ry=d-y,p,q;

	if( c==-1 && (button & 1)==1 ) c=x,d=y;
	if( c!=-1 && (button & 1)==0 )
	{
		if( x>c ) rx = x-c;
		if( y>d ) ry = y-d;

		setlinestyle(0,0,0);
		hidemouse();
		ellipse(c,d,0,360,rx,ry);
		if(mode==7)
		{
			if(fillstyle!=12)
			setfillstyle(fillstyle,color);
			else
			setfillpattern(pattern,color);
			fillellipse(c,d,rx,ry);

		}

		p=c-rx,q=d-ry;
		c+=rx,d+=ry;
		if(p>(maxx-11)||p<71||q>(maxy-31)||q<21||c>(maxx-11)||c<71||d>(maxy-31)||d<21)
		setscreen(1);

		showmouse();
		c=d=-1;
	}
}

void dirline()
{
	static int c=-1,d=-1;

	if( c==-1 && (button & 1)==1 ) c=x,d=y;
	if( c!=-1 && (button & 1)==1 )
	{
		hidemouse();
		line(c,d,x,y);
		showmouse();
		c=x,d=y;
	}
	if( c!=-1 && (button & 2)==2 ) c=-1,d=-1;
}


char *txt;

char* getstr()
{
	char ch,*s;
	int i=0;

	s=(char*)malloc(20);

	settextstyle(2,0,4);

	while( ch!=13 && i<11)
	{
		gotoxy(31+i,14);
		ch=getch();
		if(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z'||ch>='0'&&ch<='9')
		{

		s[i++]=ch;
		str[0]=ch;
		str[1]=0;

		outtextxy(225+(i*6),210,str);
		}

	}
	s[i]=0;
	return s;
}

int getfont()
{
	int i=0;
	char ch;

	while(ch!=13)
	{
		fillblock(227,195,423,233,1,8);
		fillblock(350,176,420,190,1,8);

		settextstyle(i,0,1);
		outtextxy(230,193,txt);
		settextstyle(2,0,4);

		switch(i)
		{
		case  0: outtextxy(350,179,"default");break;
		case  1: outtextxy(350,179,"triplex");break;
		case  2: outtextxy(350,179,"small" );break;
		case  3: outtextxy(350,179,"sans serif" );break;
		case  4: outtextxy(350,179,"gothic" );break;
		case  5: outtextxy(350,179,"script" );break;
		case  6: outtextxy(350,179,"simplex" );break;
		case  7: outtextxy(350,179,"triplex scr" );break;
		case  8: outtextxy(350,179,"complex" );break;
		case  9: outtextxy(350,179,"european" );break;
		case 10: outtextxy(350,179,"bold" );break;
		}

		ch=getch();

		if(ch==72) i--;
		if(ch==80) i++;
		if( i==-1) i=10;
		if( i==11) i=0;
	}
	return i;
}

int getfontsize()
{
	int i=1;
	char ch;

	while(ch!=13)
	{
		gotoxy(30,14);
		printf("%s","                     ");
		gotoxy(30,14);
		printf("%d",i);

		ch=getch();

		if(ch==72) i--;
		if(ch==80) i++;
		if( i== 0) i=10;
		if( i==11) i=0;
	}
	return i;
}

int getdirn()
{
	int i=0;
	char ch;

	while(ch!=13)
	{
		gotoxy(30,14);

		if(i)
		printf("%s","vertical             ");
		else
		printf("%s","horizontal           ");

		ch=getch();
		if( ch==72 || ch==80 ) i=!i;
	}
	return i;
}

void getstrinfo()
{
	int fnt=0,sz=1,dirn=0;

	to_buf(225,175,100,60);
	setlinestyle(0,0,0);
	show_box("enter text");
	txt=getstr();
	show_box("choose font");
	fnt=getfont();
	show_box("choose font size");
	sz=getfontsize();
	show_box("choose text direction");
	dirn=getdirn();
	from_buf(225,175,100,60);
	settextstyle(fnt,dirn,sz);
	outtextxy(x,y,txt);
	setscreen(1);
}
void erase(int sz)
{
	int a=x-sz,b=y-sz,c=x+sz,d=y+sz;

	if( a < 71 ) a = 71 ;
	if( b < 20 ) b = 21 ;
	if( c > ( maxx-11 ) ) c = maxx-11 ;
	if( d > ( maxy-30 ) ) d = maxy-31 ;

	setcolor(7);
	rectangle(a,b,c,d);
	setcolor(0);
	fillblock(a,b,c,d,1,bcol);
}

void fillit1(int,int);
char f=0;

void fillit(int p,int q)
{
	if( p>(maxx-11) || p<71 || q>(maxy-31) || q<21 || f )
	{
		f=1;
		return;
	}

	if( xycol==getpixel(p,q+1) )
	{
		putpix(p,q+1,color);
		fillit(p,q+1);
	}

	if( xycol==getpixel(p+1,q) )
	{
		putpix(p+1,q,color);
		fillit(p+1,q);
	}

	if( xycol==getpixel(p,q-1) )
	{
		putpix(p,q+1,color);
		fillit(p,q-1);
	}
}

void fillit1(int p,int q)
{
	if( p>(maxx-11) || p<71 || q>(maxy-31) || q<21 || f )
	{
		f=1;
		return;
	}

	if( xycol==getpixel(p-1,q) )
	{
		putpix(p+1,q,color);
		fillit1(p-1,q);
	}

	if( xycol==getpixel(p,q-1) )
	{
		putpix(p,q+1,color);
		fillit1(p,q-1);
	}

	if( xycol==getpixel(p+1,q) )
	{
		putpix(p+1,q,color);
		fillit(p+1,q);
	}
}

void myfill(int a,int b)
{
	hidemouse();
	if(xycol==-1)	xycol=getpixel(a,b);
//	fillit(a,b);
	fillit1(a,b);
	showmouse();
	f=0;
	xycol=-1;
}

void finished()
{
	closegraph();
	clrscr();
	for(j=0;j<16;j++)
	setpalette(j,j);

	exit(0);
}
void getuserlinepattern()
{
	int u,v;

	hidemouse();
	to_buf(80,240,75,40);
	fillblock(80,240,224,279,1,8);
	setcolor(15);
	setlinestyle(0,0,0);
	settextstyle(2,0,4);
	outtextxy(93,241,"Edit Pattern");
	setcolor(0);
	rectangle(80,240,224,279);
	line(80,255,224,255);

	for(j=88;j<216;j+=8)
	fillblock(j+1,265,j+7,271,1,0);

	for(j=208;j>86;j-=8)
	{
		if(userpat&0x8000)
		fillblock(j,264,j+8,272,1,15);

		userpat<<=1;
	}

	showmouse();

	while(1)
	{
		while(!kbhit())
		{
			setcolor(7);
			getmousepos(&button,&x,&y);

			if( x<216 && x>88 && y<272 && y>264 )
			if( (button & 1)==1 )
			{
				u=x;
				while(u%8) u--;
				v=y;
				while(v%8) v--;
				u++;v++;

				hidemouse();
				if(getpixel(u+3,v+3)==0)
				fillblock(u,v,u+6,v+6,1,15);
				showmouse();
			}
			else
			if( (button & 2)==2 )
			{
				u=x;
				while(u%8) u--;
				v=y;
				while(v%8) v--;
				u++;v++;

				hidemouse();
				if(getpixel(u+3,v+3)==15)
				fillblock(u,v,u+6,v+6,1,0);
				showmouse();
			}
		}
		if(getch()==13) break;
	}

	hidemouse();

	for(j=210;j>88;j-=8)
	{
		userpat<<=1;

		if(getpixel(j,267))
		userpat|=0x0001;
	}

	from_buf(80,240,75,40);
	highlightlinestyle();
	showmouse();
	setlinestyle(linestyle,userpat,thickness);
	setcolor(color);
}
void getuserfillpattern()
{
	char t[8];
	int a,b,u,v;
	unsigned char c;

	hidemouse();
	to_buf(80,285,60,100);
	fillblock(80,286,190,384,1,8);
	setcolor(0);
	rectangle(80,286,190,384);
	line(80,305,190,305);
	settextstyle(2,0,4);

	for(a=104;a<168;a+=8)
	for(b=312;b<376;b+=8)
	fillblock(a+1,b+1,a+7,b+7,1,0);

	setcolor(15);
	outtextxy(93,290,"Edit Pattern");

	u=0;
	setcolor(8);

	for(b=312;b<376;b+=8)
	{
		c=pattern[u++];
		for(a=104;a<168;a+=8)
		{
			if(c&0x80)
			fillblock(a,b,a+8,b+8,1,15);

			c<<=1;
		}
	}

	showmouse();

	while(1)
	{
		while(!kbhit())
		{
			setcolor(7);
			getmousepos(&button,&x,&y);

			if( x<168 && x>104 && y<376 && y>312 )
			if( (button & 1)==1 )
			{
				u=x;
				while(u%8) u--;
				v=y;
				while(v%8) v--;
				u++;v++;

				hidemouse();
				if(getpixel(u+3,v+3)==0)
				fillblock(u,v,u+6,v+6,1,15);
				showmouse();
			}
			else
			if( (button & 2)==2 )
			{
				u=x;
				while(u%8) u--;
				v=y;
				while(v%8) v--;
				u++;v++;

				hidemouse();
				if(getpixel(u+3,v+3)==15)
				fillblock(u,v,u+6,v+6,1,0);
				showmouse();
			}
		}
		if(getch()==13) break;
	}

	u=0;
	hidemouse();

	for(b=314;b<378;b+=8)
	{
		c=0x00;

		for(a=106;a<170;a+=8)
		{
			c<<=1;

			if(getpixel(a,b))
			c|=0x01;
		}
		pattern[u++]=c;
	}

	from_buf(80,285,60,100);
	fillstyle=12;
	highlightfillstyle();
	showmouse();
}
void pro1()
{
	if( (button & 1)==0 ) flag=1;

	if( x<(maxx-11) && x>71 && y<(maxy-31) && y>21 )
	{
		setfillstyle(1,getpixel(x,y-1));
		bar(5,366,65,379);


		setcolor(3);
		settextstyle(2,0,4);
		setfillstyle(1,0);

		if(a!=x)
		{
			bar(7,385,45,396);
			itoa(x,str,10);
			outtextxy(15,385,str);
		}

		if(b!=y)
		{
			bar(46,385,65,396);
			itoa(y,str,10);
			outtextxy(45,385,str);
		}
		setcolor(color);


		if(mode==4||mode==6)	rect();
		if(mode==5||mode==7)	cir();
		if(mode==8)		dirline();

		if( (button & 1)==1 )
		{
			flag=0;
			hidemouse();
			switch(mode)
			{
			case  0: putpix(x,y,color);break;
			case  1: line(a,b,x,y);break;
			case  2: dots(0);break;
			case  3: bigdots(0);break;
			case  9: getstrinfo();break;
			case 10: erase(5);break;
			case 11: myfill(x,y);break;
			}
			showmouse();
		}
		else flag=1;

		a=x,b=y;
//		if( a>(maxx-9) || a<71 || b>(maxy-29) || b<21 ) a=x,b=y;
	}

	if(mode==6||mode==7)
	if( (button & 2)==2 )
	if( x>42 && x<60 && y>274 && y<292 )
	fillstyle=12,getuserfillpattern();


	setcolor(7);
	settextstyle(2,0,4);


		if( x>340 && x<374 && y< 15 )	   pro=1;
		else if( x>385 && x<414 && y< 15 ) pro=2;
		else if( x>440 && x<500 && y< 15 ) pro=3;
		else if( x>530 && x<560 && y< 15 ) pro=4;
		else if( x>590 && x<625 && y< 15 ) pro=5;
		else pro=0;

	if(pro!=j)
	{
		hidemouse();
		setcolor(7);
		     if(j==1) outtextxy(343,2,"Image");
		else if(j==2) outtextxy(398,2,"File");
		else if(j==3) outtextxy(445,2,"Blackend");
		else if(j==4) outtextxy(534,2,"Clear");
		else if(j==5) outtextxy(597,2,"Exit");
		setcolor(15);
		     if(pro==1) outtextxy(343,2,"Image");
		else if(pro==2) outtextxy(398,2,"File");
		else if(pro==3) outtextxy(445,2,"Blackend");
		else if(pro==4) outtextxy(534,2,"Clear");
		else if(pro==5) outtextxy(597,2,"Exit");

		showmouse();
		j=pro;
	}

}

void proces(int g)
{
	int h;
	char t[4];

	if(g==0)
	{
		setcolor(10);
		settextstyle(2,0,4);
		outtextxy(100,468,"processing...   0%");
		setcolor(2);
		setlinestyle(0,userpat,1);
		for(h=101;h<201;h+=2)
		line(h,460,h,468);
		setlinestyle(linestyle,userpat,thickness);
	}
	else if(g==100)
	fillblock(100,458,200,480,1,1);
	else
	{
		if(g%2==0) g++;
		setcolor(10);
		itoa(g,t,10);
		strcat(t,"%");
		fillblock(180,470,200,478,1,1);
		outtextxy(180,468,t);
		setlinestyle(0,userpat,1);
		line(100+g,460,100+g,468);
		setlinestyle(linestyle,userpat,thickness);
	}
}
void savefile(int as)
{
	int i;

	hidemouse();
	proces(pro=0);

	if(as)
	{
		to_buf(225,175,100,60);
		setlinestyle(0,0,0);
		show_box("Enter file name");
		strcpy(filename,getstr());
		strcat(filename,".pix");
		from_buf(225,175,100,60);
	}

	fp=fopen(filename,"w");

	fputc(7,fp);
	for(i=71;i<(maxx-10);i++)
	{
		for(j=21;j<(maxy-30);j++)
		fputc(getpixel(i,j),fp);

		if(i%5==0&&pro<100) proces(pro++);
	}

	while(pro<100) proces(pro++);

	fclose(fp);
	gotoxy(10,1);
	printf("%-11s",filename);
	delay(200);
	showmouse();
	proces(100);
}

void openfile()
{
	int i;

	hidemouse();
	proces(pro=0);
	to_buf(225,175,100,60);
	setlinestyle(0,0,0);
	show_box("Enter file name");
	strcpy(filename,getstr());
	from_buf(225,175,100,60);
	strcat(filename,".pix");
	fp=fopen(filename,"r");
	fillblock(71,21,(maxx-11),(maxy-31),1,0);

	if(fgetc(fp)==7)
	for(i=71;i<(maxx-10);i++)
	{
		for(j=21;j<(maxy-30);j++)
		putpix(i,j,fgetc(fp));

		if(i%5==0&&pro<100) proces(pro++);
	}

	while(pro<100) proces(pro++);

	fclose(fp);
	gotoxy(10,1);
	printf("%-11s",filename);
	delay(200);
	showmouse();
	proces(100);
}

void newfile()
{
	hidemouse();
	strcpy(filename,"new.pix");
	gotoxy(10,1);
	printf("%-11s",filename);
	fillblock(71,21,(maxx-11),(maxy-31),1,0);
	delay(200);
	showmouse();
}

void filemenu()
{
	int n=0;

	delay(200);
	hidemouse();
	to_buf(385,17,50,84);
	fillblock(385,17,484,100,1,8);
	setcolor(7);
	setlinestyle(0,userpat,1);
	rectangle(385,17,484,100);
	setlinestyle(linestyle,userpat,thickness);

	setcolor(7);
	settextstyle(2,0,4);

	outtextxy(390,36,"New");
	outtextxy(390,52,"Open");
	outtextxy(390,68,"Save");
	outtextxy(390,84,"Save as");

	showmouse();

	while(1)
	{
		getmousepos(&button,&x,&y);

		     if(y> 36&&y< 43&&x>390&&x<415) n=1;
		else if(y> 52&&y< 60&&x>390&&x<421) n=2;
		else if(y> 68&&y< 77&&x>390&&x<421) n=3;
		else if(y> 84&&y< 94&&x>390&&x<446) n=4;
		else n=0;

		if(j!=n)
		{
		hidemouse();
		setcolor(7);
		     if(j==1) outtextxy(390,36,"New");
		else if(j==2) outtextxy(390,52,"Open");
		else if(j==3) outtextxy(390,68,"Save");
		else if(j==4) outtextxy(390,84,"Save as");
		setcolor(15);
		     if(n==1) outtextxy(390,36,"New");
		else if(n==2) outtextxy(390,52,"Open");
		else if(n==3) outtextxy(390,68,"Save");
		else if(n==4) outtextxy(390,84,"Save as");

		j=n;
		showmouse();
		}

		if( (button&1)==1 )	break;
	}

	hidemouse();
	setcolor(7);
	outtextxy(398,2,"File");
	from_buf(385,17,50,84);
	delay(200);
	showmouse();

	switch(n)
	{
	case 1: newfile(); return;
	case 2: openfile(); return;
	case 3: savefile(0); return;
	case 4: savefile(1);
	}
	fcloseall();
}


void verticalflip()
{
	int i;
	FILE *t;

	proces(pro=0);
	t=fopen("t~","w");
	hidemouse();

	for(j=21;j<(maxy-30);j++)
	{
		if(j%4==0&&pro<40) proces(pro++);

		for(i=71;i<(maxx-10);i++)
		fputc(getpixel(i,j),t);
	}

	proces(pro++);

	fclose(fp);
	fillblock(71,21,maxx-11,maxy-31,1,0);
	t=fopen("t~","r");

	proces(pro++);

	for(j=(maxy-32);j>20;j--)
	{
		for(i=71;i<(maxx-10);i++)
		putpix(i,j,fgetc(t));

		if(j%6==0&&pro<100) proces(pro++);
	}

	fclose(t);
	showmouse();
	unlink("t~");
	proces(100);
}

void horizontalflip()
{
	int i;
	FILE *t;

	proces(pro=0);
	t=fopen("t~","w");
	hidemouse();

	for(i=71;i<(maxx-10);i++)
	{
		for(j=21;j<(maxy-30);j++)
		fputc(getpixel(i,j),t);

		if(i%3==0&&pro<37) proces(pro++);
	}

	proces(pro++);
	fclose(fp);
	proces(pro++);
	fillblock(71,21,maxx-11,maxy-31,1,0);
	proces(pro++);
	t=fopen("t~","r");
	proces(pro++);

	for(i=(maxx-12);i>70;i--)
	{
		if(i%6==0&&pro<100) proces(pro++);

		for(j=21;j<(maxy-30);j++)
		putpix(i,j,fgetc(t));
	}

	fclose(t);
	showmouse();
	unlink("t~");
	proces(100);
}

void rotateleft()
{
	int i;
	FILE *t;

	proces(pro=0);
	t=fopen("t~","w");
	hidemouse();

	for(i=0;i<418;i++)
	{
		for(j=0;j<418;j++)
		fputc(getpixel(i+71,j+21),t);

		if(i%4==0&&pro<42) proces(pro++);
	}

	fclose(fp);
	proces(pro++);
	fillblock(71,21,maxx-11,maxy-31,1,0);
	proces(pro++);
	t=fopen("t~","r");
	proces(pro++);

	for(j=417;j>=0;j--)
	{
		if(j%6==0&&pro<100) proces(pro++);

		for(i=0;i<418;i++)
		putpix(i+71,j+21,fgetc(t));
	}

	fclose(t);
	showmouse();
	unlink("t~");
	proces(100);
}

void rotateright()
{
	int i;
	FILE *t;

	proces(pro=0);
	t=fopen("t~","w");
	hidemouse();

	for(i=0;i<418;i++)
	{
		if(i%5==0&&pro<34) proces(pro++);

		for(j=0;j<418;j++)
		fputc(getpixel(i+71,j+21),t);
	}

	fclose(fp);
	fillblock(71,21,maxx-11,maxy-31,1,0);
	t=fopen("t~","r");

	for(j=0;j<418;j++)
	{
		if(j%6==0&&pro<100) proces(pro++);

		for(i=417;i>=0;i--)
		putpix(i+71,j+21,fgetc(t));
	}

	fclose(t);
	showmouse();
	unlink("t~");
	proces(100);
}

void image()
{
	int n=0;

	delay(200);
	hidemouse();
	to_buf(330,17,75,85);
	fillblock(330,17,479,100,1,8);
	setcolor(7);
	setlinestyle(0,userpat,1);
	rectangle(330,17,479,100);
	setlinestyle(linestyle,userpat,thickness);

	setcolor(7);
	settextstyle(2,0,4);

	outtextxy(340,36,"Rotate Left");
	outtextxy(340,52,"Rotate Right");
	outtextxy(340,68,"Vertical Flip");
	outtextxy(340,84,"Horizontal Flip");

	showmouse();

	while(1)
	{
		getmousepos(&button,&x,&y);

		     if(y> 36&&y< 43&&x>335&&x<420) n=1;
		else if(y> 52&&y< 60&&x>335&&x<428) n=2;
		else if(y> 68&&y< 77&&x>335&&x<440) n=3;
		else if(y> 84&&y< 94&&x>335&&x<455) n=4;
		else n=0;


		if(j!=n)
		{
		hidemouse();
		setcolor(7);
		     if(j==1) outtextxy(340,36,"Rotate Left");
		else if(j==2) outtextxy(340,52,"Rotate Right");
		else if(j==3) outtextxy(340,68,"Vertical Flip");
		else if(j==4) outtextxy(340,84,"Horizontal Flip");

		setcolor(15);
		     if(n==1) outtextxy(340,36,"Rotate Left");
		else if(n==2) outtextxy(340,52,"Rotate Right");
		else if(n==3) outtextxy(340,68,"Vertical Flip");
		else if(n==4) outtextxy(340,84,"Horizontal Flip");
		j=n;
		showmouse();
		}

		if( (button&1)==1 )	break;
	}

	hidemouse();
	setcolor(7);
	outtextxy(343,2,"Image");
	from_buf(330,17,75,85);
	delay(200);
	showmouse();

	switch(n)
	{
	case 1: rotateleft(); return;
	case 2: rotateright(); return;
	case 3: verticalflip(); return;
	case 4: horizontalflip();
	}
}


void pro2()
{
	if( x>340 && x<374 && y< 15 )
	{
		image();
		return;
	}

	if( x>385 && x<414 && y< 15 )
	{
		filemenu();
		return;
	}

	if( x>590 && x<625 && y< 15 )
	{
		finished();
		return;
	}

	if( x>530 && x<560 && y< 15 )
	{
		fillblock(71,21,maxx-11,maxy-31,1,bcol=15);
		return;
	}

	if( x>440 && x<500 && y< 15 )
	{
		fillblock(71,21,maxx-11,maxy-31,1,bcol=0);
		return;
	}

	if( x>250 && y<481 && y>459 )
	{
		colrblock(getpixel(x-2,y-2));
		return;
	}

	if( x>0 && x<62 && y>20 && y<209 )
	{
		mode=((y+13)/32)*2-1;
		if( x>0 && 30>x) mode--;
		if(m!=mode)  highlightmode(),m=mode;
	}

	if(mode==6||mode==7)
	if( x>5 && x<60 && y>220 && y<292 )
	{
		fillstyle  = ((x+13)/18);
		fillstyle += ((y-220+13)/18-1)*3;

		if( fillstyle <  1 ) fillstyle =  1 ;
		if( fillstyle > 12 ) fillstyle = 12 ;
		if( p!=fillstyle )
		{
			hidemouse();
			highlightfillstyle();
			showmouse();
			p=fillstyle;
		}
	}
}


void pro3()
{
	static char pressed;

	linestyle  = ((x+13)/18);
	linestyle += ((y-220+13)/18-1)*3;
	linestyle--;

	if( linestyle==4 && (button&2)==2 )
	{
		l=linestyle=4;
		getuserlinepattern();
	}

	if( linestyle==5 && (button&2)==2 )
	{
		pressed=0;
		thickness++;
		if( thickness == 4 ) thickness = 0 ;
		linestyle=l;
		hidemouse();
		highlightlinestyle();
		showmouse();
		setcolor(color);
		return;
	}

	if( linestyle < 0 ) linestyle = 0 ;
	if( linestyle > 4 ) linestyle = 4 ;

	if( l!=linestyle )
	{
		hidemouse();
		highlightlinestyle();
		showmouse();
		l=linestyle;
		setcolor(color);
	}
}

void defcol()
{
	setpalette(  1 ,  72);
	setpalette(  2 , 130);
	setpalette(  3 ,  43);
	setpalette(  4 , 198);
	setpalette(  5 ,  77);
	setpalette(  6 ,  52);
	setpalette(  7 ,  15);
	setpalette(  9 ,  73);
	setpalette( 10 ,  18);
	setpalette( 12 ,  36);
	setpalette( 13 ,  45);
	setpalette( 14 ,  54);
}


void main()
{
	setscreen(0);
	restrictmouse(1,1,maxx-1,maxy-1);
	initmouse();
	showmouse();
	defcol();

	while(1)
	{
		getmousepos(&button,&x,&y);
		pro1();

		if( flag && (button&1)==1 ) pro2();

		if(mode==4||mode==8)
		if( x>5 && x<60 && y>220 && y<256 )
		if( flag && (button&1)==1 || (button&2)==2 ) pro3();
	}
}