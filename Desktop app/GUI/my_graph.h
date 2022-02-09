/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  05.08.2021
 */
#include <std_lib_facilities.h>
#include "Graph.h"
#include "GUI.h"
#include <functional>

typedef function<double(double)> Fct_std;

const Graph_lib::Font def_font{ Graph_lib::Font::times_bold };
const int def_fnt_size{ 20 };
const int binary_tree_min_n{ 0 };
const int binary_tree_min_w{ 10 };
const int binary_tree_min_h{ 10 };

//----------------------------------------------------------------------------

extern double fac(int n);
extern Point angle_point(Point xy, int radius, double azimut);
extern Point rotate_point(Point center, Point p, double angle);

//----------------------------------------------------------------------------

enum class Connect_line {
	simple, down_arrow, up_arrow,
	text_simple, text_down_arrow, text_up_arrow
};

//----------------------------------------------------------------------------

struct Arc : Graph_lib::Shape {
	Arc(Point p, int ww, int hh, double deg1, double deg2)
		: w{ ww }, h{ hh }, d1{ deg1 }, d2{ deg2 } 
	{ 
		add(Point{ p.x - w, p.y - h });
	}

	void draw_lines()	const;

	Point center()	const { return Point{ point(0).x + w, point(0).y + h }; }

	void set_fangle(double deg);
	double fangle()	const { return d1; }

	void set_sangle(double deg);
	double sangle() const { return d2; }

	void set_major(int ww)
	{
		set_point(0, Point{ center().x - ww, center().y - h });
		w = ww;
	}
	int major()		const { return w; }

	void set_minor(int hh)
	{
		set_point(0, Point{ center().x - w, center().y - hh });
		h = hh;
	}
	int minor()		const { return h; }
private:
	int w;
	int h;
	double d1;
	double d2;
};

//----------------------------------------------------------------------------

struct Box : Graph_lib::Shape {
	Box() :w{ 2 }, h{ 2 }, r{ 1 } { }
	Box(Point p, int ww, int hh, int rr)
		: w{ ww }, h{ hh }, r{ rr }
	{
		if (w <= r || h <= r) error("sides must be bigger then radius");
		add(p); 
	}

	void draw_lines()	const;

	void set_width(int ww)
	{
		if (ww <= 0) error("width must be bigger then zero");
		if (ww <= r) error("side must be bigger then radius");
		w = ww;
	}
	int width()			const { return w; }

	void set_height(int hh)
	{
		if (hh <= 0) error("high must be bigger then zero");
		if (hh <= r) error("side must be bigger then radius");
		h = hh;
	}
	int height()			const { return h; }

	void set_radius(int rr)
	{
		if (rr <= 0) error("radius must be bigger then zero");
		if (w <= rr || h <= rr) error("radius must be smaller then sides");
		r = rr;
	}
	int radius()		const { return r; }
private:
	int w;
	int h;
	int r;
};

struct Text_box : Box {
	Text_box(Point p, const string& s)
		:lab(s), fnt(def_font), fnt_sz(def_fnt_size),
		 Box(p, int(s.size() + 3) * 0.45 * def_fnt_size, 
			 def_fnt_size * 2, def_fnt_size / 2)
	{	}

	void draw_lines() const;

	void set_text(const string& s) 
	{ 
		lab = s; 
		Box::set_width(int(s.length() + 1) * fnt_sz / 2); 
	}
	string text() const { return lab; }

	void set_font(Graph_lib::Font f) { fnt = f; }
	Graph_lib::Font font() const { return Graph_lib::Font(fnt); }

	void set_font_size(int s) 
	{ 
		fnt_sz = s;
		Box::set_height(s * 2);
		Box::set_radius(s / 2);
		Box::set_width(int(lab.length() + 1) * s / 2);
	}
	int font_size() const { return fnt_sz; }
private:
	string lab;
	Graph_lib::Font fnt;
	int fnt_sz;
};

//----------------------------------------------------------------------------

struct Arrow : Graph_lib::Line {
	Arrow(Point p1, Point p2, int ll, int ww)
		:Line{ p1, p2 }, l{ ll }, w{ ww }
	{
		if (l <= 0) error("length should be greater then zero");
		if (w <= 0) error("width should be greater then zero");
	}

	void draw_lines() const;
private:
	int w;
	int l;
};

//----------------------------------------------------------------------------

Point n(Graph_lib::Rectangle);
Point s(Graph_lib::Rectangle);
Point e(Graph_lib::Rectangle);
Point w(Graph_lib::Rectangle);

Point center(Graph_lib::Rectangle);

Point ne(Graph_lib::Rectangle);
Point se(Graph_lib::Rectangle);
Point sw(Graph_lib::Rectangle);
Point nw(Graph_lib::Rectangle);

//----------------------------------------------------------------------------

Point n(Graph_lib::Circle);
Point s(Graph_lib::Circle);
Point e(Graph_lib::Circle);
Point w(Graph_lib::Circle);

Point ne(Graph_lib::Circle*);
Point se(Graph_lib::Circle*);
Point sw(Graph_lib::Circle*);
Point nw(Graph_lib::Circle*);

//----------------------------------------------------------------------------

Point n(Graph_lib::Ellipse*);
Point s(Graph_lib::Ellipse*);
Point e(Graph_lib::Ellipse*);
Point w(Graph_lib::Ellipse*);

//----------------------------------------------------------------------------

struct Regular_hexagon : Graph_lib::Shape {
	Regular_hexagon(Point p, int rr);	// center and radius

	void draw_lines() const;

	Point center() const;

	void set_radius(int rr);
	int radius() const { return r; }

	void set_height(int hh);
	int height() const { return h; }
private:
	int r;
	int h;
};

//----------------------------------------------------------------------------

struct Regular_polygon : Graph_lib::Closed_polyline {
	Regular_polygon(Point p, int nn, int rr)
		:n{ nn }, r{ rr }
	{ 
		if (r <= 0) error("radius should be bigger then zero");
		if (n < 3) r = 3;
		add(Point{ p.x, p.y - r });
		point_search();
	}

	void set_center(Point p)
	{
		set_point(0, Point{ p.x, p.y - r });
		point_search();
	}
	Point center() const { return Point(point(0).x, point(0).y + r); }

	void set_radius(int rr)
	{
		if (r <= 0) error("radius should be bigger then zero");
		set_point(0, Point{ center().x, center().y - rr });
		r = rr;
		point_search();
	}
	int radius() const { return r; }

	void set_cornum(int nn)
	{
		n = nn;
		if (nn < 3) r = 3;
		point_search();
	}
	int cornum() const { return n; }
protected:
	void point_search();

	int n;
	int r;
};

//----------------------------------------------------------------------------

struct Right_triangle : Graph_lib::Shape {
	Right_triangle(Point p1, Point p2, double dd)
		: deg{ dd }
	{
		if (int(deg) % 180 == 0) error("angle shouldn't equal n*180");
		add(p1);
		add(p2); 
	}

	void draw_lines() const;

	int radius()	const
	{
		return sqrt(pow(point(0).x - point(1).x, 2) +
					pow(point(0).y - point(1).y, 2)) / 2;
	}

	Point center()	const
	{
		return Point{ (point(0).x + point(1).x) / 2, 
					  (point(0).y + point(1).y) / 2 };
	}

	void set_angle(double dd) { deg = dd; }
	double angle()	const	 { return deg; }
private:
	double deg;
};

//----------------------------------------------------------------------------

struct Poly : Graph_lib::Polygon {
	Poly(initializer_list<Point> lst);

	struct Simp_func_coef {
		Simp_func_coef(double kk, double bb)
			:k{ kk }, b{ bb } { }

		double k;
		double b;
	};
	
	struct Mpoint {
		Mpoint(double xx, double yy)
			:x{ xx }, y{ yy } { }
		Mpoint(Point p)
			:x{ double(p.x) }, y{ double(p.y) } { }

		double x;
		double y;
	};
};

/* Возвращает коэффициенты простого линейного уравнения, построенного по двум точкам отрезка */
Poly::Simp_func_coef get_sfnc_coef(const int& x1, const int& y1, const int& x2, const int& y2);

/* Возвращает точку, в которой пересекаются две линии, построенные по коэффициентам */
Poly::Mpoint get_cross_point(const Point& p1, const Point& p2, const Point& p3, const Point& p4);

/* Проверяет, принадлежит ли точка пересечения двух линий заданным отрезкам */
bool segments_overlap(const Point& p1, const Point& p2, const Point& p3, const Point& p4);

/* Проверяет, параллелльны ли отрезки друг другу */
bool parallel_segments(const Point& p1, const Point& p2, const Point& p3, const Point& p4);

/* Проверяет, лежит ли точка в прямоугольнике, образованном диагональю между точками p1 и p2 */
bool point_in_cube(const Point& p1, const Point& p2, const Poly::Mpoint& p3);

//----------------------------------------------------------------------------

struct Star : Regular_polygon {
	using Regular_polygon::Regular_polygon;

	void point_search();
};

//----------------------------------------------------------------------------

struct Smiley : Graph_lib::Circle {
	using Circle::Circle;
	void draw_lines() const;
};

struct Hat_smiley : Smiley {
	using Smiley::Smiley;
	void draw_lines() const;
};

struct Frowny : Graph_lib::Circle {
	using Circle::Circle;
	void draw_lines() const;
};

struct Hat_frowny : Frowny {
	using Frowny::Frowny;
	void draw_lines() const;
};

//----------------------------------------------------------------------------

struct Immobile_circle : Graph_lib::Circle {
	using Circle::Circle;
private:
	void move(int dx, int dy) { }
};

//----------------------------------------------------------------------------

struct Striped_rectangle : Graph_lib::Rectangle {
	using Rectangle::Rectangle;
	void draw_lines() const;
};

//----------------------------------------------------------------------------

struct Striped_circle : Graph_lib::Circle {
	using Circle::Circle;
	void draw_lines() const;
};

//----------------------------------------------------------------------------

struct Striped_closed_polyline : Graph_lib::Closed_polyline {
	using Closed_polyline::Closed_polyline;
	void draw_lines() const;
};

//----------------------------------------------------------------------------

struct Octagon : Regular_polygon {
	Octagon(Point p, int rr): Regular_polygon(p, 8, rr) { }
protected:
	using Shape::set_point;
private:
	using Regular_polygon::set_cornum;
	using Shape::add;
};

//----------------------------------------------------------------------------

struct Group : Graph_lib::Shape {
	void add_shape(Shape& s) { shapes.push_back(s); }
	void add_shape(Shape* p) { shapes.push_back(p); }

	void draw() const;
	void move(int dx, int dy);

	void set_color(Graph_lib::Color col);
	void set_style(Graph_lib::Line_style sty);
	void set_fill_color(Graph_lib::Color col);

	int size() const { return shapes.size(); }

	Point point() const 
	{
		if (shapes.size() <= 0) error("there is no points in group");
		return shapes[0].point(0); 
	}
protected:
	Shape::number_of_points;
	void move(int i, int x, int y)
	{
		is_range(i);
		shapes[i].move(x, y);
	}
	void set_color(int i, Graph_lib::Color col)
	{
		is_range(i);
		shapes[i].set_color(col);
	}
	void set_style(int i, Graph_lib::Line_style sty)
	{
		is_range(i);
		shapes[i].set_style(sty);
	}
	void set_fill_color(int i, Graph_lib::Color col)
	{
		is_range(i);
		shapes[i].set_fill_color(col);
	}

	Shape& operator[] (int i) { return shapes[i]; }

	void clear() { shapes.clear(); }
private:
	Shape::set_point;

	void is_range(int i) const { if(shapes.size() <= i) error("group range error"); }

	Graph_lib::Vector_ref<Shape> shapes;
};

//----------------------------------------------------------------------------

struct Chess_desk : Group {
	Chess_desk(Point p, int size);

	int size()		const { return s; }
	
	void set_center(Point p);
	Point center()	const { return { point().x + s / 2, point().y + s / 2 }; }

	Point sector(int x, int y) const;
protected:
	Group::size;
	Group::add_shape;
	Group::set_color;
	Group::set_style;
	Group::set_fill_color;
private:
	int s;
};

//----------------------------------------------------------------------------

struct Square_desk : Group {
	Square_desk(Point p, int size, int number);

	int size()		const { return s; }
	int number()	const { return n; }

	void set_center(Point p);
	Point center()	const { return {point().x + s / 2, point().y + s / 2}; }

	Point sector(int x, int y) const;

	Group::set_color;
	Group::set_fill_color;
protected:
	Group::size;
	Group::add_shape;
private:
	int s;
	int n;
};

//----------------------------------------------------------------------------

struct Pseudo_window : Group {
	Pseudo_window(Point p, int width, int height, const string& s);

	int width()		const { return w; }
	int height()	const { return h; }
private:
	int w;
	int h;
};

//----------------------------------------------------------------------------

struct Binary_tree : Graph_lib::Shape {
	Binary_tree(Point p, int levels, int width, int height);
	Binary_tree(Point p, int levels, int width, int height, Connect_line line);
	Binary_tree(Point p, int levels, int width, int height, Graph_lib::Color color);
	Binary_tree(Point p, int levels, int width, int height, Graph_lib::Color color, Connect_line line);

	void draw_lines() const;

	void add_text(int level, int position, const string& label);

	Point point(int i, int j) const;
protected:
	virtual void units() const;

	void calc();

	void styled_line(Point begin, Point end) const;
 
	Shape::point;
	Shape::set_color;
private:
	Shape::set_fill_color;
	Shape::set_point;

	Graph_lib::Vector_ref<Graph_lib::Text> marks;
	Connect_line lstyle = Connect_line::simple;
	Point p0;
	int fnt_size{ 10 };
	int n;
	int w;
	int h;
};

//----------------------------------------------------------------------------

struct Binary_tree_triangle : Binary_tree {
	Binary_tree::Binary_tree;
	void units() const;
};

//----------------------------------------------------------------------------

struct Controller {
	virtual void on() = 0;
	virtual void off() = 0;
	virtual void set_level(int) = 0;
	virtual void show() const = 0;
};

struct Test_control : Controller {
	Test_control() { }

	void on()	{ state = true; }
	void off()	{ state = false; }

	void set_level(int l) { level = l; }

	void show() const
	{
		cout << "\tState: <" << (state ? "on" : "off") << ">; "
			 << "Level: [ " << level << " ]\n";
	}
private:
	bool state{ false };
	int level{ 0 };
};

struct Color_control : Controller {
	Color_control(Graph_lib::Shape* p)
		: ptr{ p } { }
	
	void on()	{ ptr->set_color(Graph_lib::Color::visible); }
	void off()	{ ptr->set_color(Graph_lib::Color::invisible); }

	void set_level(int l)
	{
		if (l < 0) error("color is a positive number");
		ptr->set_color(Graph_lib::Color(l % 256));
	}

	void show() const 
	{
		cout << "State: <" << (ptr->color().visibility() == 0 ? "off" : "on") << ">; "
			 << "Color: [ " << ptr->color().as_int() << " ]\n";
	}
private:
	Graph_lib::Shape* ptr;
};

//----------------------------------------------------------------------------
struct Func : Graph_lib::Shape {
	template<class T>
	Func(Fct_std f, double r1, double r2, Point orig, int count = 100,
		 double xscale = 25, double yscale = 25, T precision = 100)
		:func{ f }, a{ r1 }, b{ r2 }, x_sc{ xscale }, y_sc{ yscale }, 
		 prec{ double(100 - precision) / 100 }, p_num{ count }, zero{ orig }
	{
		if (r2 - r1 <= 0) error("bad graphing range");
		if (count <= 0) error("non-positive graphing count");
		if (1 < prec || prec < 0) error("bad precision");
		calc();
	}

	void set_scale(double x_scale = INFINITY, double y_scale = INFINITY);
	double x_scale() const { return x_sc; }
	double y_scale() const { return y_sc; }

	void set_ends(double r1 = INFINITY, double r2 = INFINITY);
	double left()	const { return a; }
	double right()	const { return b; }

	void set_count(int p)
	{
		if (p <= 0) error("non-positive graphing count");
		p_num = p;
		calc();
	}
	int count() const { return p_num; }

	void set_orig(Point orig) { zero = orig; calc(); }
	Point orig() const { return zero; }

	void set_func(Fct_std f) { func = f; calc(); }

	bool accurate() const;
protected:
	void calc();
private:
	Fct_std func;
	double x_sc, y_sc;
	double a, b;
	double prec;
	int p_num;
	Point zero;
};

//----------------------------------------------------------------------------

class Scale {
	int cbase;
	int vbase;
	double scale;
public:
	Scale(int b, int vb, double s) :cbase(b), vbase(vb), scale(s)
	{ }
	int operator() (int v) const
	{
		return cbase + (v - vbase) * scale;
	}
};

//----------------------------------------------------------------------------

struct Histogram : Group {
	Histogram(Point left_bottom, int width = 100, int height = 50,
			  initializer_list<double> list = {}, string name = "histogram")
		:zero(left_bottom), histname{ name }, w{ width }, h{ height }
	{
		bad_width(w);
		bad_height(h);

		add_data(list);
	}

	void add_data(initializer_list<double> list, bool do_calc = true);
	void add_data(double data, bool  do_calc = true);
	void set_data(int i, double data);

	int number_of_column()	const	{ return dat.size(); }
	
	void set_left_bottom(Point xy);
	Point left_bottom()		const	{ return zero; }
	
	void set_width(int l);
	int width()		const	{ return w; }
	
	void set_height(int l);
	int height()	const	{ return h; }

	void bad_width(int l) { if (l < dat.size()) error(er_width); }
	void bad_height(int l) { if (l < 10) error(er_height); }

	void mark_on()	{ marked = true; calc(); }
	void mark_off() { marked = false; calc(); }
	bool is_marked() const { return marked; }

	void add_name(int i, int x, int y, const string& s, 
				  Graph_lib::Color = Graph_lib::Color::black);
	void add_name(initializer_list<string> list, bool do_calc = true);
	void add_name(string name, bool do_calc = true);
	void set_name(int i, const string& s);

	void set_mark_color(Graph_lib::Color c) { m_col = c; }
	void set_name_color(Graph_lib::Color c) { n_col = c; }
protected:
	Group::add_shape;

	void draw_mark(int x, int y, const string& s, Graph_lib::Color);
	void calc();
private:
	vector<double> dat;
	vector<string> names;
	Graph_lib::Color n_col{ Graph_lib::Color::black };
	Graph_lib::Color m_col{ Graph_lib::Color::black };
	Point zero;
	int w, h;
	bool marked{ false };
	int m_size{ 15 };

	string histname;
	const string er_width{ histname + ": bad histogram width" };
	const string er_height{ histname + ": bad histogram height" };
	const string er_datvec{ histname + ": data vector range error" };
	const string er_datnam{ histname + ": too many names" };
	const string er_namvec{ histname + ": name vector range error" };
};

class Histo_data {
	vector<double> c;
	vector<string> n;

	const string his_name{ "hist data class" };
	const string er_namvec{ his_name + ": name vector range error" };
	const string er_colvec{ his_name + ": column vector range error" };
protected:
	void column_range(int i) const { if (c.size() <= i) error(er_colvec); }
	void name_range(int i)	 const { if (n.size() <= i) error(er_namvec); }
public:
	double data(int i) const	 { column_range(i); return c[i]; }
	void add_column(double data) { c.push_back(data); }

	string name(int i) const	{ name_range(i); return n[i]; }
	void add_name(string name)	{ n.push_back(name); }

	int size() const { return c.size(); }
};

istream& operator>> (istream&, Histo_data&);
void histogram_load(Histogram* load_to, const string& file_from);


//----------------------------------------------------------------------------

class Clock_arr : public Graph_lib::Shape {
	Shape::fill_color;
	Shape::move;
	Shape::number_of_points;
	Shape::set_color;
	Shape::set_fill_color;
	Shape::set_point;
	Shape::set_style;
	Shape::style;

	int max_t;
protected:
	Shape::point;
	Shape::color;

	int r, t;

	enum Arr_type {
		sec = 59, min = 3599, hour = 23
	};
public:
	Clock_arr(Point xy, int radius, Arr_type tp)
			:r{ radius }, t{ 0 }, max_t{ int(tp) }
	{ add(xy); }

	void draw_lines() const = 0;

	void set(int cur)
	{
		if (max_t < cur || cur < 0) error("wrong set time");
		t = cur;
	}

	void tick() { t == max_t ? t = 0 : ++t; }

	int t_max()		const { return max_t;}
	int current()	const { return t; }
};

struct Sec_arr : public Clock_arr {
	Sec_arr(Point xy, int radius, int height)
		:Clock_arr(xy, radius, Clock_arr::sec), 
		h{ height} { }

	void draw_lines() const;
private:
	int h;
};

struct Min_arr : public Clock_arr {
	Min_arr(Point xy, int radius)
		:Clock_arr(xy, radius, Clock_arr::min) { }
	
	void draw_lines() const;

	double is_hour() const { if (t==0) return true; return false; }
};

struct Hour_arr : public Clock_arr {
	Hour_arr(Point xy, int radius)
		:Clock_arr(xy, radius, Clock_arr::hour) { }

	void draw_lines() const;
};

//----------------------------------------------------------------------------

struct Airplane : Graph_lib::Lines {
	Airplane(Point xy, int l);

	void rotate(double angle);

	void move(int x, int y)
	{
		c.x += x;
		c.y += y;

		Shape::move(x, y);
	}

	void init(Point xy);

	Point center() const { return c; }
	Point max() const
	{
		int xmax = point(0).x;
		int ymax = point(0).y;
		for (int i{ 0 }; i < number_of_points(); ++i) {
			if (xmax < point(i).x) xmax = point(i).x;
			if (ymax < point(i).y) ymax = point(i).y;
		}
		return {xmax, ymax};
	}
	Point min() const
	{
		int xmin = point(0).x;
		int ymin = point(0).y;
		for (int i{ 0 }; i < number_of_points(); ++i) {
			if (xmin > point(i).x) xmin = point(i).x;
			if (ymin > point(i).y) ymin = point(i).y;
		}
		return {xmin, ymin};
	}
	void set_azimut(double azimut)
	{
		alph = azimut;
	}
	double azimut() const { return alph; }
private:
	Lines::add;
	Shape::fill_color;
	Shape::point;
	Shape::set_fill_color;
	Shape::set_point;
	
	Point c;
	int l;
	double alph{ 270 };
};

//----------------------------------------------------------------------------

class Plane_engine {
	Airplane* bort;
	int xm, ym;
	bool phisics{false};

	double dx { 1 };
	double k{ 1 };
	bool xdir{ false }, ydir{ false };

	void set_course();
	void set_azimut();
	void fix_position();
public:
	Plane_engine(Airplane& plane, int xmax, int ymax) 
		: bort{ &plane }, xm{ xmax }, ym{ ymax } 
		{ 
			set_azimut();
		}

	void fly();

	void damage_on() { phisics = true; }
	void damage_off(){ phisics = false; }
	bool is_damage() const { return phisics; }
};

//----------------------------------------------------------------------------
