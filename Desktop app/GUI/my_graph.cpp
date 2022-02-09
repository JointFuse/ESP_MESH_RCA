/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  05.08.2021
 */
#include "my_graph.h"

const double pi{ 3.14159265 };

//----------------------------------------------------------------------------

double fac(int n) { return n > 1 ? n * fac(n - 1) : 1; }

//----------------------------------------------------------------------------

extern void draw_arrow(Point p0, Point p1);

//----------------------------------------------------------------------------

void Arc::draw_lines() const
{
	if (fill_color().visibility()) {
		fl_color(fill_color().as_int());
		fl_pie(point(0).x, point(0).y, w + w, h + h, d1, d2);
	}
	if (color().visibility()) {
		fl_color(color().as_int());
		fl_arc(point(0).x, point(0).y, w + w, h + h, d1, d2);
	}
}

void Arc::set_fangle(double deg)
{
	if (deg > d2) error("first angle must smaller then second");
	d1 = deg;
}

void Arc::set_sangle(double deg)
{
	if (deg < d1) error("second angle must be bigger then first");
	d2 = deg;
}

//----------------------------------------------------------------------------

void Box::draw_lines() const
{
	if (fill_color().visibility()) {
		fl_color(fill_color().as_int());
		fl_rectf(point(0).x, point(0).y + r, w, h - 2 * r);
		fl_rectf(point(0).x + r, point(0).y, w - 2 * r, h);
		fl_pie(point(0).x, point(0).y, r + r, r + r, 90, 180);
		fl_pie(point(0).x + w - 2 * r, point(0).y, r + r, r + r, 0, 90);
		fl_pie(point(0).x + w - 2 * r, point(0).y + h - 2 * r, r + r, r + r, 270, 360);
		fl_pie(point(0).x, point(0).y + h - 2 * r, r + r, r + r, 180, 270);
	}
	if (color().visibility()) {
		fl_color(color().as_int());
		fl_line(point(0).x + r, point(0).y, point(0).x + w - r, point(0).y);
		fl_line(point(0).x + r, point(0).y + h, point(0).x + w - r, point(0).y + h);
		fl_line(point(0).x, point(0).y + r, point(0).x, point(0).y + h - r);
		fl_line(point(0).x + w, point(0).y + r, point(0).x + w, point(0).y + h - r);
		fl_arc(point(0).x, point(0).y, r + r, r + r, 90, 180);
		fl_arc(point(0).x + w - 2 * r, point(0).y, r + r, r + r, 0, 90);
		fl_arc(point(0).x + w - 2 * r, point(0).y + h - 2 * r, r + r, r + r, 270, 360);
		fl_arc(point(0).x, point(0).y + h - 2 * r, r + r, r + r, 180, 270);
	}
}

void Text_box::draw_lines() const
{
	Box::draw_lines();

	int ofnt = fl_font();
	int osz = fl_size();
	fl_color(56);
	fl_font(fnt.as_int(), fnt_sz);
	fl_draw(lab.c_str(), point(0).x + Box::radius(), 
			point(0).y + Box::height() - Box::radius() * 1.3);
	fl_font(ofnt, osz);
}

//----------------------------------------------------------------------------

void Arrow::draw_lines() const
{
	if (color().visibility()) {
		double L{ sqrt(pow(point(0).x - point(1).x, 2) + pow(point(0).y - point(1).y, 2)) };

		double mid_x = l / L * point(0).x + (1 - l / L) * point(1).x;
		double mid_y = l / L * point(0).y + (1 - l / L) * point(1).y;

		double al_x = mid_x + w / L * (point(1).y - point(0).y);
		double al_y = mid_y + w / L * (point(0).x - point(1).x);

		double ar_x = mid_x + w / L * (point(0).y - point(1).y);
		double ar_y = mid_y + w / L * (point(1).x - point(0).x);

		fl_line(point(0).x, point(0).y, point(1).x, point(1).y);
		fl_line(point(1).x, point(1).y, al_x, al_y);
		fl_line(point(1).x, point(1).y, ar_x, ar_y);
	}
}

//----------------------------------------------------------------------------

Point n(Graph_lib::Rectangle r)
{
	return Point{ r.point(0).x + r.width() / 2, r.point(0).y };
}

Point s(Graph_lib::Rectangle r)
{
	return Point{ r.point(1).x - r.width() / 2, r.point(1).y };
}

Point e(Graph_lib::Rectangle r)
{
	return Point{ r.point(1).x, r.point(1).y - r.height() / 2 };
}

Point w(Graph_lib::Rectangle r)
{
	return Point{ r.point(0).x, r.point(0).y + r.height() / 2 };
}

Point center(Graph_lib::Rectangle r)
{
	return Point{ r.point(0).x + r.width() / 2, 
				  r.point(0).y + r.height() / 2 };
}

Point ne(Graph_lib::Rectangle r)
{
	return Point{ r.point(1).x, r.point(0).y };
}

Point se(Graph_lib::Rectangle r)
{
	return Point{ r.point(1).x, r.point(1).y };
}

Point sw(Graph_lib::Rectangle r)
{
	return Point{ r.point(0).x, r.point(1).y };
}

Point nw(Graph_lib::Rectangle r)
{
	return Point{ r.point(0).x, r.point(0).y };
}

//----------------------------------------------------------------------------

Point n(Graph_lib::Circle c)
{
	return Point{ c.center().x, c.center().y - c.radius() };
}

Point s(Graph_lib::Circle c)
{
	return Point{ c.center().x, c.center().y + c.radius() };
}

Point e(Graph_lib::Circle c)
{
	return Point{ c.center().x + c.radius(), c.center().y };
}

Point w(Graph_lib::Circle c)
{
	return Point{ c.center().x - c.radius(), c.center().y };
}

Point ne(Graph_lib::Circle* c)
{
	return Point{ c->center().x + int(c->radius() * sin(45 * pi / 180)),
				  c->center().y - int(c->radius() * sin(45 * pi / 180)) };
}

Point se(Graph_lib::Circle* c)
{
	return Point{ c->center().x + int(c->radius() * sin(45 * pi / 180)),
				  c->center().y + int(c->radius() * sin(45 * pi / 180)) };
}

Point sw(Graph_lib::Circle* c)
{
	return Point{ c->center().x - int(c->radius() * sin(45 * pi / 180)),
				  c->center().y + int(c->radius() * sin(45 * pi / 180)) };
}

Point nw(Graph_lib::Circle* c)
{
	return Point{ c->center().x - int(c->radius() * sin(45 * pi / 180)),
				  c->center().y - int(c->radius() * sin(45 * pi / 180)) };
}

//----------------------------------------------------------------------------

Point n(Graph_lib::Ellipse* e)
{
	return Point{ e->center().x, e->center().y - e->minor() };
}

Point s(Graph_lib::Ellipse* e)
{
	return Point{ e->center().x, e->center().y + e->minor() };
}

Point e(Graph_lib::Ellipse* e)
{
	return Point{ e->center().x + e->major(), e->center().y };
}

Point w(Graph_lib::Ellipse* e)
{
	return Point{ e->center().x - e->major(), e->center().y };
}

//----------------------------------------------------------------------------

Regular_hexagon::Regular_hexagon(Point p, int rr)
	:r{rr}, h{ int(r * sin(60 * pi / 180)) }
{
	add(Point{ p.x - r, p.y - h });
}

Point Regular_hexagon::center() const
{
	return Point{ point(0).x + r, point(0).y + h };
}

void Regular_hexagon::set_radius(int rr)
{
	r = rr;
	h = rr * sin(60 * pi / 180);
}

void Regular_hexagon::set_height(int hh)
{
	h = hh;
	r = hh * 1.0 / sin(60 * pi / 180);
}

void Regular_hexagon::draw_lines() const
{
	if (fill_color().visibility()) {
		fl_color(fill_color().as_int());
		fl_begin_complex_polygon();
		fl_vertex(point(0).x + r * 0.5, point(0).y);
		fl_vertex(point(0).x + r * 1.5, point(0).y);
		fl_vertex(center().x + r, center().y);
		fl_vertex(center().x + r * 0.5, center().y + h);
		fl_vertex(center().x - r * 0.5, center().y + h);
		fl_vertex(center().x - r, center().y);
		fl_end_complex_polygon();
		fl_color(color().as_int());    // reset color
	}
	if (color().visibility()) {
		fl_line(point(0).x + r *0.5, point(0).y, point(0).x + r * 1.5, point(0).y);
		fl_line(point(0).x + r * 1.5, point(0).y, center().x + r, center().y);
		fl_line(center().x + r, center().y, center().x + r * 0.5, center().y + h);
		fl_line(center().x + r * 0.5, center().y + h, center().x - r * 0.5, center().y + h);
		fl_line(center().x - r * 0.5, center().y + h, center().x - r, center().y);
		fl_line(center().x - r, center().y, point(0).x + r * 0.5, point(0).y);
	}
}

//----------------------------------------------------------------------------

void Regular_polygon::point_search()
{
	Point zero{ point(0) };
	clear_points();
	add(zero);

	double alph{ 2 * pi / n };

	for (int i{ 1 }; i < n; ++i) {
		int x = center().x + r * cos(alph * i + pi * 1.5);
		int y = center().y + r * sin(alph * i + pi * 1.5);
		add(Point{ x, y });
	}
}

//----------------------------------------------------------------------------

void Right_triangle::draw_lines() const
{
	if (!(color().visibility() || fill_color().visibility()))
		return;

	double azimut = acos((point(1).x - center().x) * 1.0 / radius());
	if (asin((point(1).y - center().y) * 1.0 / radius()) < 0)
		azimut = 2 * pi - azimut;

	Point p3{ center().x + int(radius() * cos(azimut + deg * pi / 180)),
			  center().y + int(radius() * sin(azimut + deg * pi / 180)) };

	if (fill_color().visibility()) {
		fl_color(fill_color().as_int());
		fl_begin_complex_polygon();
		fl_vertex(point(0).x, point(0).y);
		fl_vertex(point(1).x, point(1).y);
		fl_vertex(p3.x, p3.y);
		fl_end_complex_polygon();
		fl_color(color().as_int());    // reset color
	}

	if (color().visibility()) {
		fl_line(point(0).x, point(0).y, point(1).x, point(1).y);
		fl_line(point(1).x, point(1).y, p3.x, p3.y);
		fl_line(p3.x, p3.y, point(0).x, point(0).y);
	}
}

//----------------------------------------------------------------------------

Poly::Simp_func_coef get_sfnc_coef(const int& x1, const int& y1, const int& x2, const int& y2)
{
	if (y1 == y2) return Poly::Simp_func_coef{ 0, double(y1) };
	if (x1 == x2) return Poly::Simp_func_coef{ INFINITY, double(x1) };

	double b = (x1 * y2 - x2 * y1) * 1.0 / (x1 - x2);
	double k = (y1 - b) * 1.0 / x1;
	if (x1 == 0) k = (y2 - b) * 1.0 / x2;
	else		 k = (y1 - b) * 1.0 / x1;

	return Poly::Simp_func_coef{ k, b };
}

Poly::Mpoint get_cross_point(const Point& p1, const Point& p2, const Point& p3, const Point& p4)
{
	Poly::Simp_func_coef cf1{ get_sfnc_coef(p1.x, p1.y, p2.x, p2.y) };
	Poly::Simp_func_coef cf2{ get_sfnc_coef(p3.x, p3.y, p4.x, p4.y) };

	if (cf1.k == cf2.k && cf1.b != cf2.b) error("parallel lines");

	if (cf1.k == INFINITY) return Poly::Mpoint{ cf1.b, cf2.k * cf1.b + cf2.b };
	if (cf2.k == INFINITY) return Poly::Mpoint{ cf2.b, cf1.k * cf2.b + cf1.b };

	if (cf1.k == 0) return Poly::Mpoint{ (cf1.b - cf2.b) / cf2.k, cf1.b };
	if (cf2.k == 0) return Poly::Mpoint{ (cf2.b - cf1.b) / cf1.k, cf2.b };

	double x = (cf2.b - cf1.b) * 1.0 / (cf1.k - cf2.k);
	double y = cf1.k * x + cf1.b;

	return Poly::Mpoint{ x, y };
}

bool segments_overlap(const Point& p1, const Point& p2, const Point& p3, const Point& p4)
try{
	if (parallel_segments(p1, p2, p3, p4)) return false;

	Poly::Mpoint cross_point{ get_cross_point(p1, p2, p3, p4) };
	
	if (point_in_cube(p1, p2, cross_point) && point_in_cube(p3, p4, cross_point))
		return true;

	return false;
}
catch (exception& e) {
	if (e.what() != "simular segments") throw e;

	if (point_in_cube(p1, p2, p3) || point_in_cube(p1, p2, p4) ||
		point_in_cube(p3, p4, p1) || point_in_cube(p3, p4, p2))
		return true;
	
	return false;
}

bool parallel_segments(const Point& p1, const Point& p2, const Point& p3, const Point& p4)
{
	Poly::Simp_func_coef cf1{ get_sfnc_coef(p1.x, p1.y, p2.x, p2.y) };
	Poly::Simp_func_coef cf2{ get_sfnc_coef(p3.x, p3.y, p4.x, p4.y) };

	if (cf1.k == cf2.k && cf1.b == cf2.b) error("simular segments");
	if (cf1.k == cf2.k) return true;
	return false;
}

bool point_in_cube(const Point& p1, const Point& p2, const Poly::Mpoint& p3)
{
	double x1, x2, x3{ p3.x };
	if (p1.x < p2.x) { x1 = p1.x; x2 = p2.x; }
	else			 { x1 = p2.x; x2 = p1.x; }

	double y1, y2, y3{ p3.y };
	if (p1.y < p2.y) { y1 = p1.y; y2 = p2.y; }
	else			 { y1 = p2.y; y2 = p1.y; }

	if ((x1 <= x3 && x3 <= x2) && (y1 <= y3 && y3 <= y2))
		return true;

	return false;
}

Poly::Poly(initializer_list<Point> lst)
{
	if (lst.size() < 3) error("too few points in Poly");
	for (Point p : lst) add(p);

	int n = number_of_points();
	for (int i{ 0 }; i < n; ++i)
		for (int j{ 0 }; j < n; ++j) {
			if (j == i - 1 || j == i + 1 || j == i ||
				(j == n - 1 && i == 0) || 
				(j == 0 && i == n - 1))
				continue;

			if (segments_overlap(point(i), point(i == n - 1 ? 0 : i + 1),
								 point(j), point(j == n - 1 ? 0 : j + 1)))
				error("crossing lines in Poly");
		}
}

//----------------------------------------------------------------------------

void Star::point_search()
{
	Point zero{ point(0) };
	clear_points();
	add(zero);

	double alph{ 2 * pi / n };
	int x, y;

	for (int i{ 1 }; i < n; ++i) {
		x = center().x + r * 0.35 * cos(alph * i + pi - alph * 0.5);
		y = center().y + r * 0.35 * sin(alph * i + pi - alph * 0.5);
		add(Point{ x, y });

		x = center().x + r * cos(alph * i + pi);
		y = center().y + r * sin(alph * i + pi);
		add(Point{ x, y });
	}

	x = center().x + r * 0.35 * cos(alph * n + pi - alph * 0.5);
	y = center().y + r * 0.35 * sin(alph * n + pi - alph * 0.5);
	add(Point{ x, y });
}

//----------------------------------------------------------------------------

void Smiley::draw_lines() const
{
	Circle::draw_lines();

	if (color().visibility() || fill_color().visibility()) {
		fl_color(color().as_int());
		fl_arc(point(0).x + radius() * 0.25, point(0).y + radius() * 0.5, radius() * 0.5, radius() * 0.5, 0, 360);
		fl_arc(point(0).x + radius() * 1.25, point(0).y + radius() * 0.5, radius() * 0.5, radius() * 0.5, 0, 360);
		fl_arc(point(0).x, point(0).y + radius() * 0.5, radius() * 2, radius(), 220, 320);
	}
}

void Hat_smiley::draw_lines() const
{
	Smiley::draw_lines();

	if (color().visibility() || fill_color().visibility()) {
		int y = point(0).y + radius() * (1 - sin(pi / 3));
		
		fl_color(color().as_int());
		fl_begin_complex_polygon();
		fl_vertex(point(0).x + radius() * 0.5, y - radius() * 0.25);
		fl_vertex(point(0).x + radius() * 1.5, y - radius() * 0.25);
		fl_vertex(point(0).x + radius() * 1.5, y);
		fl_vertex(point(0).x + radius() * 0.5, y);
		fl_end_complex_polygon();

		fl_line(point(0).x, y - radius() * 0.15, point(0).x + radius() * 0.5, y);
		fl_line(point(0).x + radius() * 2, y - radius() * 0.15, point(0).x + radius() * 1.5, y);
	}
}

void Frowny::draw_lines() const
{
	Circle::draw_lines();

	if (color().visibility() || fill_color().visibility()) {
		fl_color(color().as_int());
		fl_arc(point(0).x + radius() * 0.25, point(0).y + radius() * 0.5, radius() * 0.5, radius() * 0.5, 0, 360);
		fl_arc(point(0).x + radius() * 1.25, point(0).y + radius() * 0.5, radius() * 0.5, radius() * 0.5, 0, 360);
		fl_arc(point(0).x, point(0).y + radius() * 1.25, radius() * 2, radius(), 40, 140);
	}
}

void Hat_frowny::draw_lines() const
{
	Frowny::draw_lines();

	if (color().visibility() || fill_color().visibility()) {
		int y = point(0).y + radius() * (1 - sin(pi / 3));

		fl_color(color().as_int());
		fl_begin_complex_polygon();
		fl_vertex(point(0).x + radius() * 0.5, y - radius() * 0.25);
		fl_vertex(point(0).x + radius() * 1.5, y - radius() * 0.25);
		fl_vertex(point(0).x + radius() * 1.5, y);
		fl_vertex(point(0).x + radius() * 0.5, y);
		fl_end_complex_polygon();

		fl_line(point(0).x, y + radius() * 0.15, point(0).x + radius() * 0.5, y);
		fl_line(point(0).x + radius() * 2, y + radius() * 0.15, point(0).x + radius() * 1.5, y);
	}
}

//----------------------------------------------------------------------------

void Striped_rectangle::draw_lines() const
{
	if (fill_color().visibility()) {    // fill
		fl_color(fill_color().as_int());

		for (int i{ point(0).y + 2 }; i < point(0).y + height(); i += 2)
			fl_line(point(0).x, i, point(0).x + width() -1, i);

		fl_color(color().as_int());		// reset color
	}

	if (color().visibility()) {			// lines on top of fill
		fl_color(color().as_int());
		fl_rect(point(0).x, point(0).y, width(), height());
	}
}

//----------------------------------------------------------------------------

void Striped_circle::draw_lines() const
{
	if (fill_color().visibility()) {	// fill
		fl_color(fill_color().as_int());
		
		int dx;
		double alph;
		for (int i{ point(0).y + 2 }; i < point(0).y + radius() * 2; i += 2) {
			alph = asin((i - center().y) * 1.0 / radius());
			dx = radius() * cos(alph);
			fl_line(center().x - dx, i, center().x + dx - 1, i);
		}

		fl_color(color().as_int());		// reset color
	}

	if (color().visibility()) {
		fl_color(color().as_int());
		fl_arc(point(0).x, point(0).y, r + r, r + r, 0, 360);
	}
}

//----------------------------------------------------------------------------

void Striped_closed_polyline::draw_lines() const
{
	if (fill_color().visibility()) {	// fill
		fl_color(fill_color().as_int());

		int y_min{ point(0).y }, y_max{ point(0).y };
		for (int i{ 1 }; i < number_of_points(); ++i) {
			if (point(i).y < y_min) y_min = point(i).y;
			if (y_max < point(i).y) y_max = point(i).y;
		}
		
		Graph_lib::Vector_ref<Graph_lib::Line> lines;
		for (int i{ 0 }; i < number_of_points() - 1; ++i) {
			if (point(i).y < point(i + 1).y)
				lines.push_back(new Graph_lib::Line(point(i), point(i + 1)));
			else
				lines.push_back(new Graph_lib::Line(point(i + 1), point(i)));
		}

		if (point(0).y < point(number_of_points() - 1).y)
			lines.push_back(new Graph_lib::Line(point(0), point(number_of_points() - 1)));
		else
			lines.push_back(new Graph_lib::Line(point(number_of_points() - 1), point(0)));
		
		for (int y_line{ y_min }; y_line < y_max; y_line += 4) {
			vector<int> x_pts;
			for (int i{ 0 }; i < lines.size(); ++i) {
				if (lines[i].point(0).y < y_line && y_line < lines[i].point(1).y) {
					Poly::Mpoint p(get_cross_point(Point{ 1, y_line }, Point{ 10, y_line },
													lines[i].point(0), lines[i].point(1)));
					x_pts.push_back(p.x);
				}
				if ((y_line == lines[i].point(1).y &&
					lines[i].point(1).y == lines[i == (lines.size() - 1) ? 0 : i + 1].point(0).y) ||
					(y_line == lines[i].point(0).y &&
					 lines[i].point(0).y == lines[i == (lines.size() - 1) ? 0 : i + 1].point(1).y)) {
					Poly::Mpoint p(get_cross_point(Point{ 1, y_line }, Point{ 10, y_line },
													lines[i].point(0), lines[i].point(1)));
					x_pts.push_back(p.x);
				}
			}
			sort(x_pts.begin(), x_pts.end());
			for (int i{ 0 }; i < int(x_pts.size()) - 1; i += 2) {
				fl_line(x_pts[i], y_line, x_pts[i + 1], y_line);
			}
		}

		fl_color(color().as_int());	// reset color
	}

	if (color().visibility()) {
		Shape::draw_lines();
		fl_line(point(number_of_points() - 1).x,
			point(number_of_points() - 1).y,
			point(0).x,
			point(0).y);
	}
}

//----------------------------------------------------------------------------

void Group::draw() const
{
	for (int i{ 0 }; i < shapes.size(); ++i)
		shapes[i].draw();
}

void Group::move(int x, int y)
{
	for (int i{ 0 }; i < shapes.size(); ++i)
		shapes[i].move(x, y);
}

void Group::set_color(Graph_lib::Color col)
{
	for (int i{ 0 }; i < shapes.size(); ++i)
		shapes[i].set_color(col);
}

void Group::set_style(Graph_lib::Line_style sty)
{
	for (int i{ 0 }; i < shapes.size(); ++i)
		shapes[i].set_style(sty);
}

void Group::set_fill_color(Graph_lib::Color col)
{
	for (int i{ 0 }; i < shapes.size(); ++i)
		shapes[i].set_fill_color(col);
}

//----------------------------------------------------------------------------

Chess_desk::Chess_desk(Point p, int size)
	: s{ size }
{
	if (s < 16) error("too small size (min 16)");

	for (int i{ 0 }; i < 8; ++i)
		for (int j{ 0 }; j < 8; ++j) {
			add_shape(new Graph_lib::Rectangle({ p.x + i * s / 8, p.y + j * s / 8 }, s / 8, s / 8));

			if ((i + j) % 2 == 0) set_fill_color(Group::size() - 1, Graph_lib::Color::white);
			else set_fill_color(Group::size() - 1, Graph_lib::Color::black);

			set_color(Group::size() - 1, Graph_lib::Color::invisible());
		}
}

void Chess_desk::set_center(Point p)
{
	int dx{ p.x - s / 2 - point().x };
	int dy{ p.y - s / 2 - point().y };

	move(dx, dy);
}

Point Chess_desk::sector(int x, int y) const
{
	if (8 < x || x < 1 || 8 < y || y < 1) error("position out of desk");
	return { point().x + (x - 1) * s / 8, point().y + (y - 1) * s / 8 };
}

//----------------------------------------------------------------------------

Pseudo_window::Pseudo_window(Point p, int width, int height, const string& s)
	: w{width >= 140 ? width : 140}, h{height >= 120 ? height : 120}
{
	add_shape(new Box(p, w, h, 10));
	set_color(size() - 1, Graph_lib::Color::invisible);
	set_fill_color(size() - 1, Graph_lib::Color::cyan);
	add_shape(new Box({ p.x + 5, p.y + 30 }, w - 10, h - 35, 5));
	set_color(size() - 1, Graph_lib::Color::invisible);
	set_fill_color(size() - 1, Graph_lib::Color::dark_yellow);
	add_shape(new Graph_lib::Text({ p.x + 10, p.y + 20 }, s));
	add_shape(new Graph_lib::Rectangle({ p.x + w - 40, p.y }, { p.x + w - 10, p.y + 20 }));
	set_color(size() - 1, Graph_lib::Color::invisible);
	set_fill_color(size() - 1, Graph_lib::Color::red);
	add_shape(new Graph_lib::Rectangle({ p.x + w - 64, p.y }, { p.x + w - 40, p.y + 20 }));
	set_color(size() - 1, Graph_lib::Color::invisible);
	set_fill_color(size() - 1, Graph_lib::Color::green);
	add_shape(new Graph_lib::Rectangle({ p.x + w - 88, p.y }, { p.x + w - 64, p.y + 20 }));
	set_color(size() - 1, Graph_lib::Color::invisible);
	set_fill_color(size() - 1, Graph_lib::Color::yellow);
	add_shape(new Graph_lib::Line({ p.x + w - 32, p.y + 4 }, { p.x + w - 18, p.y + 16 }));
	set_style(size() - 1, Graph_lib::Line_style(Graph_lib::Line_style::solid, 3));
	set_color(size() - 1, Graph_lib::Color::white);
	add_shape(new Graph_lib::Line({ p.x + w - 32, p.y + 16 }, { p.x + w - 18, p.y + 4 }));
	set_style(size() - 1, Graph_lib::Line_style(Graph_lib::Line_style::solid, 3));
	set_color(size() - 1, Graph_lib::Color::white);
	add_shape(new Graph_lib::Rectangle({ p.x + w - 58, p.y + 4 }, 13, 13));
	set_style(size() - 1, Graph_lib::Line_style(Graph_lib::Line_style::solid, 3));
	set_color(size() - 1, Graph_lib::Color::white);
	add_shape(new Graph_lib::Rectangle({ p.x + w - 83, p.y + 10 }, { p.x + w - 69, p.y + 16 }));
	set_fill_color(size() - 1, Graph_lib::Color::white);
}

//----------------------------------------------------------------------------

Binary_tree::Binary_tree(Point p, int levels, int width, int height)
	: p0{ p }, n{ levels }, w{ width }, h{ height }
{
	if (n < binary_tree_min_n) n = binary_tree_min_n;
	if (w < binary_tree_min_w) w = binary_tree_min_w;
	if (h < binary_tree_min_h) h = binary_tree_min_h;
	calc();
}

Binary_tree::Binary_tree(Point p, int levels, int width, int height, Connect_line line)
	: p0{ p }, n{ levels }, w{ width }, h{ height }, lstyle{ line }
{
	if (n < binary_tree_min_n) n = binary_tree_min_n;
	if (w < binary_tree_min_w) w = binary_tree_min_w;
	if (h < binary_tree_min_h) h = binary_tree_min_h;
	calc();
}

Binary_tree::Binary_tree(Point p, int levels, int width, int height, Graph_lib::Color color)
	: p0{ p }, n{ levels }, w{ width }, h{ height }
{
	if (n < binary_tree_min_n) n = binary_tree_min_n;
	if (w < binary_tree_min_w) w = binary_tree_min_w;
	if (h < binary_tree_min_h) h = binary_tree_min_h;
	calc();
	set_color(color);
}

Binary_tree::Binary_tree(Point p, int levels, int width, int height, Graph_lib::Color color, Connect_line line)
	: p0{ p }, n{ levels }, w{ width }, h{ height }, lstyle{ line }
{
	if (n < binary_tree_min_n) n = binary_tree_min_n;
	if (w < binary_tree_min_w) w = binary_tree_min_w;
	if (h < binary_tree_min_h) h = binary_tree_min_h;
	calc();
	set_color(color);
}

void Binary_tree::add_text(int l, int p, const string& s)
{
	l = n - l;
	p -= 1;

	marks.push_back(new Graph_lib::Text({ point(l, p).x, point(l, p).y - 3 }, s));
	marks[marks.size() - 1].set_font_size(fnt_size);
}

Point Binary_tree::point(int i, int j) const
{
	if (i < 0 || j < 0) error("binary_tree: i or j below zero");
	
	int sum{ 0 };
	for (int x{ 0 }; x < i; ++x)
		sum += pow(2, n - 1 - x);

	if (number_of_points() < sum + j + 1) error("binary_tree: out of matrix");

	return point(sum + j);
}

void Binary_tree::units() const
{
	for (int i{ 0 }; i < number_of_points(); ++i) {
		fl_pie(point(i).x - 2, point(i).y - 2, 4, 4, 0, 360);
	}
}

void Binary_tree::calc()
{
	for (int i{ n - 1 }; i >= 0; --i)
		for (int j{ 0 }; j < pow(2, i); ++j)
			add({ p0.x + int((pow(2, n - i - 1) - 1) * 0.5 * w) + j * w * int(pow(2, n - i - 1)), p0.y + i * h });
}

void Binary_tree::styled_line(Point begin, Point end) const
{
	int space_for_text{ 25 };
	int text_x = (100 - space_for_text) * 1.0 / 100 * end.x + space_for_text * 1.0 / 100 * begin.x;
	int text_y = (100 - space_for_text) * 1.0 / 100 * end.y + space_for_text * 1.0 / 100 * begin.y;

	switch (lstyle) {
	case Connect_line::simple:
		fl_line(begin.x, begin.y, end.x, end.y);
		return;
	case Connect_line::down_arrow:
		draw_arrow(begin, end);
		return;
	case Connect_line::up_arrow:
		draw_arrow(end, begin);
		return;
	case Connect_line::text_simple:
		fl_line(begin.x, begin.y, text_x, text_y);
		return;
	case Connect_line::text_down_arrow:
		draw_arrow(begin, { text_x, text_y });
		return;
	case Connect_line::text_up_arrow:
		draw_arrow({ text_x, text_y }, begin);
		return;
	default:
		error("unknown connect line style");
	}
}

void Binary_tree::draw_lines() const
{
	if (color().visibility()) {
		units();

		for (int i{ 0 }; i < marks.size(); ++i)
			marks[i].draw_lines();

		for (int i{ n - 1 }; i > 0; --i)
			for (int j{ 0 }; j < pow(2, n - 1 - i); ++j) {
				styled_line(point(i, j), point(i - 1, 2 * j));
				styled_line(point(i, j), point(i - 1, 2 * j + 1));
			}
	}
}

//----------------------------------------------------------------------------

void Binary_tree_triangle::units() const
{
	for (int i{ 0 }; i < number_of_points(); ++i) {
		fl_begin_complex_polygon();
		fl_vertex(point(i).x, point(i).y - 3);
		fl_vertex(point(i).x - 3, point(i).y + 2);
		fl_vertex(point(i).x + 3, point(i).y + 2);
		fl_end_complex_polygon();
	}
}

//----------------------------------------------------------------------------

void draw_arrow(Point p0, Point p1)
{
	int l{ 8 }, w{ 3 };

	double L{ sqrt(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2)) };

	double mid_x = l / L * p0.x + (1 - l / L) * p1.x;
	double mid_y = l / L * p0.y + (1 - l / L) * p1.y;

	double al_x = mid_x + w / L * (p1.y - p0.y);
	double al_y = mid_y + w / L * (p0.x - p1.x);

	double ar_x = mid_x + w / L * (p0.y - p1.y);
	double ar_y = mid_y + w / L * (p1.x - p0.x);

	fl_line(p0.x, p0.y, p1.x, p1.y);
	fl_begin_complex_polygon();
	fl_vertex(p1.x, p1.y);
	fl_vertex(al_x, al_y);
	fl_vertex(ar_x, ar_y);
	fl_end_complex_polygon();
}

//----------------------------------------------------------------------------

void Func::set_scale(double x_scale, double y_scale)
{
	if (x_scale != INFINITY) x_sc = x_scale;
	if (y_scale != INFINITY) y_sc = y_scale;
	calc();
}

void Func::set_ends(double r1, double r2)
{
	if (r1 != INFINITY) a = r1;
	if (r2 != INFINITY) b = r2;
	if (b - a <= 0) error("bad graphing range");
	calc();
}

void Func::calc()
{
	double dist = (b - a) / p_num;
	double r = a;
	for (int i = 0; i < p_num; ++i) {
		add(Point(zero.x + int(r * x_sc), zero.y - int(func(r) * y_sc)));
		r += dist;
	}
}

bool Func::accurate() const
{
	for (int i{ 0 }; i < number_of_points(); ++i)
		if (abs(1 - double(point(i).y) / func(point(i).x)) < prec)
			return false;

	return true;
}

//----------------------------------------------------------------------------

void Histogram::add_data(initializer_list<double> list, bool do_calc)
{ 
	for (auto d : list)
		dat.push_back(d);
	
	if(do_calc)
		calc();
}

void Histogram::add_data(double d, bool do_calc)
{
	dat.push_back(d);

	if (do_calc)
		calc();
}

void Histogram::set_data(int i, double data)
{
	if (dat.size() - 1 < i) error(er_datvec);
	dat[i] = data;

	calc();
}

void Histogram::set_left_bottom(Point xy)
{
	int dx = xy.x - zero.x;
	int dy = xy.y - zero.y;
	move(dx, dy);
	zero = xy;
}

void Histogram::set_width(int l)
{
	bad_width(l);
	w = l;
	calc();
}

void Histogram::set_height(int l)
{
	bad_height(l);
	h = l;
	calc();
}

void Histogram::calc()
{
	if (dat.size() == 0) return;
	clear();

	constexpr double offset{ 0.1 };

	int step = w * 1.0 / dat.size();
	int xpos = zero.x;
	double yscale = dat[0];
	double ymin = dat[0] - dat[0] * offset;
	
	for (auto y : dat) {
		if (yscale < y) yscale = y;
		if (y < ymin) ymin = y - y * offset;
	}

	yscale = h * 1.0 / (yscale - ymin);

	for (int i{ 0 }; i < dat.size(); ++i) {
		add_shape(new Graph_lib::Rectangle({ xpos, zero.y - int((dat[i] - ymin) * yscale) },
											{ xpos + step, zero.y }));
		if (marked) {
			ostringstream m;
			m << setprecision(2) << fixed << dat[i];
			draw_mark(zero.x + 4, zero.y - int((dat[i] - ymin) * yscale), m.str(), m_col);
		}

		if (i < names.size())
			draw_mark(xpos, zero.y + 15, names[i], n_col);

		xpos += step;
	}
}

void Histogram::draw_mark(int x, int y, const string& s, Graph_lib::Color col)
{
	Graph_lib::Text* mark = new Graph_lib::Text({ x, y }, s);
	mark->set_font_size(m_size);
	mark->set_color(col);
	add_shape(mark);
}

void Histogram::add_name(initializer_list<string> list, bool do_calc)
{
	int i{ 0 };
	for (auto name : list) {
		if (dat.size() <= i) error(er_datnam);
		names.push_back(name);
		++i;
	}

	if(do_calc)
		calc();
}

void Histogram::add_name(string n, bool do_calc)
{
	names.push_back(n);

	if (do_calc)
		calc();
}

void Histogram::set_name(int i, const string& s)
{
	if (names.size() <= i) error(er_namvec);
	names[i] = s;
	calc();
}

void Histogram::add_name(int i, int x, int y, const string& s,
						 Graph_lib::Color col)
{
	--i;
	if (dat.size() <= i) error(er_datvec);
	draw_mark(x, y, s, col);

	int xpos = zero.x + w * (i + 0.5) / dat.size();
	double ypos = dat[0];

	for (auto y : dat)
		if (ypos < y) ypos = y;

	ypos = zero.y - h * dat[i] / ypos / 2;

	add_shape(new Graph_lib::Line({ x, y }, { xpos, int(ypos) }));
}

//----------------------------------------------------------------------------

istream& operator>> (istream& is, Histo_data& hc)
{
	char ch1, ch2, ch3;
	double d;
	string n{ "" };
	is >> ch1;

	if (is.eof())
		return is;
	
	while (is >> ch2) {
		if (ch2 == ')') {
			is.clear(ios_base::failbit);
			return is;
		}
		if (ch2 == ',') break;
		n += ch2;
	}

	is >> d >> ch3;

	if (ch1 != '(' || ch2 != ',' || ch3 != ')') {
		is.clear(ios_base::failbit);
		return is;
	}

	hc.add_column(d);
	hc.add_name(n);

	return is;
}

//----------------------------------------------------------------------------

void histogram_load(Histogram* load_to, const string& file)
{
	ifstream load_from(file);
	if (!load_from) error("histo_load: can't open file: " + file);

	Histo_data buf;
	while (load_from >> buf) { }
	if (!load_from.eof())
		error("histo_load: bad file read");

	bool is_end(false);
	for (int i{ 0 }; i < buf.size(); ++i) {
		load_to->add_data(buf.data(i), is_end);
		if (i == buf.size() - 1) is_end = true;
		load_to->add_name(buf.name(i), is_end);
	}
}

//----------------------------------------------------------------------------

Square_desk::Square_desk(Point p, int ss, int nn)
	:s{ ss }, n{ nn }
{
	if (s / n < 2) error("too small size (min 2 * number)");

	for (int i{ 0 }; i < n; ++i)
		for (int j{ 0 }; j < n; ++j) {
			add_shape(new Graph_lib::Rectangle({ p.x + j * s / n, p.y + i * s / n }, s / n, s / n));

			set_fill_color(Group::size() - 1, Graph_lib::Color::white);
		}

	set_color(Graph_lib::Color::invisible());
}

//----------------------------------------------------------------------------

Point angle_point(Point xy, int r, double a)
{
	if (r <= 0) error("angle point: radius should be positive");
	int x = xy.x + r * cos(a * 2 * pi);
	int y = xy.y + r * sin(a * 2 * pi);
	return {x, y};
}

//----------------------------------------------------------------------------

void Sec_arr::draw_lines() const
{
	double step = 0.1 / (t_max() + 1);
	double azimut = 0.75 + 1.0 * (t + 0.05) / (t_max() + 1);
	Point next;

	fl_color(Graph_lib::Color::red);
	fl_begin_complex_polygon();
	for (int i{ 0 }; i < 10; ++i) {
		next = angle_point(point(0), r - 1, azimut);
		fl_vertex(next.x, next.y);
		azimut += step;
	}
	for (int i{ 0 }; i < 10; ++i) {
		azimut -= step;
		next = angle_point(point(0), r + h, azimut);
		fl_vertex(next.x, next.y);
	}
	fl_end_complex_polygon();
	fl_color(color().as_int());
}

void Min_arr::draw_lines() const
{
	double azimut = 0.75 + 1.0 * t / (t_max() + 1);
	Point p0 = angle_point(point(0), 0.15 * r, azimut + 0.5);
	Point p1 = angle_point(point(0), 0.95 * r, azimut);

	int l{ int(0.95 * r) }, w{ 5 };

	double L{ sqrt(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2)) };

	double mid_x = l / L * p0.x + (1 - l / L) * p1.x;
	double mid_y = l / L * p0.y + (1 - l / L) * p1.y;

	double al_x = mid_x + w / L * (p1.y - p0.y);
	double al_y = mid_y + w / L * (p0.x - p1.x);

	double ar_x = mid_x + w / L * (p0.y - p1.y);
	double ar_y = mid_y + w / L * (p1.x - p0.x);

	fl_begin_complex_polygon();
	fl_vertex(p1.x, p1.y);
	fl_vertex(al_x, al_y);
	fl_vertex(p0.x, p0.y);
	fl_vertex(ar_x, ar_y);
	fl_end_complex_polygon();
}

void Hour_arr::draw_lines() const
{
	double azimut = 0.75 + 1.0 * t / (t_max() + 1) * 2;
	Point p1 = angle_point(point(0), 0.7 * r, azimut);

	int l{ int(0.5 * r) }, w{ 11 };

	double L{ sqrt(pow(point(0).x - p1.x, 2) + pow(point(0).y - p1.y, 2)) };

	double mid_x = l / L * point(0).x + (1 - l / L) * p1.x;
	double mid_y = l / L * point(0).y + (1 - l / L) * p1.y;

	double al_x = mid_x + w / L * (p1.y - point(0).y);
	double al_y = mid_y + w / L * (point(0).x - p1.x);

	double ar_x = mid_x + w / L * (point(0).y - p1.y);
	double ar_y = mid_y + w / L * (p1.x - point(0).x);

	fl_begin_complex_polygon();
	fl_vertex(p1.x, p1.y);
	fl_vertex(al_x, al_y);
	fl_vertex(ar_x, ar_y);
	fl_end_complex_polygon();
}

//----------------------------------------------------------------------------

Airplane::Airplane(Point xy, int length) : c{ xy }, l{ length }
{
	init(c);
}

void Airplane::rotate(double a)
{
	for (int i{ 0 }; i < number_of_points(); ++i)
		set_point(i, rotate_point(c, point(i), a));
}

void Airplane::init(Point xy)
{
	if(number_of_points() != 0) Shape::clear_points();

	add({ xy.x - int(0.1 * l), xy.y + int(0.32 * l) }, { xy.x - int(0.05 * l), xy.y - int(0.23 * l) });
	add({ xy.x + int(0.1 * l), xy.y + int(0.32 * l) }, { xy.x + int(0.05 * l), xy.y - int(0.23 * l) });
	add({ xy.x - int(0.05 * l), xy.y - int(0.23 * l) }, { xy.x, xy.y - int(0.5 * l) });
	add({ xy.x + int(0.05 * l), xy.y - int(0.23 * l) }, { xy.x, xy.y - int(0.5 * l) });
	add({ xy.x - int(0.05 * l), xy.y - int(0.23 * l) }, { xy.x, xy.y - int(0.36 * l) });
	add({ xy.x + int(0.05 * l), xy.y - int(0.23 * l) }, { xy.x, xy.y - int(0.36 * l) });
	add({ xy.x - int(0.1 * l), xy.y + int(0.32 * l) }, { xy.x - int(0.05 * l), xy.y + int(0.45 * l) });
	add({ xy.x + int(0.1 * l), xy.y + int(0.32 * l) }, { xy.x + int(0.05 * l), xy.y + int(0.45 * l) });
	add({ xy.x - int(0.05 * l), xy.y + int(0.45 * l) }, { xy.x + int(0.05 * l), xy.y + int(0.45 * l) });
	add({ xy.x - int(0.32 * l), xy.y + int(0.18 * l) }, { xy.x - int(0.05 * l), xy.y - int(0.23 * l) });
	add({ xy.x + int(0.32 * l), xy.y + int(0.18 * l) }, { xy.x + int(0.05 * l), xy.y - int(0.23 * l) });
	add({ xy.x - int(0.32 * l), xy.y + int(0.18 * l) }, { xy.x - int(0.1 * l), xy.y + int(0.32 * l) });
	add({ xy.x + int(0.32 * l), xy.y + int(0.18 * l) }, { xy.x + int(0.1 * l), xy.y + int(0.32 * l) });
	add({ xy.x - int(0.1 * l), xy.y + int(0.32 * l) }, { xy.x - int(0.18 * l), xy.y + int(0.5 * l) });
	add({ xy.x + int(0.1 * l), xy.y + int(0.32 * l) }, { xy.x + int(0.18 * l), xy.y + int(0.5 * l) });
	add({ xy.x - int(0.18 * l), xy.y + int(0.5 * l) }, { xy.x - int(0.05 * l), xy.y + int(0.45 * l) });
	add({ xy.x + int(0.18 * l), xy.y + int(0.5 * l) }, { xy.x + int(0.05 * l), xy.y + int(0.45 * l) });

	alph = 270;
}

//----------------------------------------------------------------------------

double get_azimut(Point c, Point p)
{
	int rad = sqrt(pow(p.x - c.x, 2) + pow(p.y - c.y, 2));

	double azimut = acos((p.x - c.x) * 1.0 / rad);
	if (asin((p.y - c.y) * 1.0 / rad) < 0)
		azimut = 2 * pi - azimut;

	return azimut;
}

//----------------------------------------------------------------------------

Point rotate_point(Point c, Point p, double angle)
{
	int rad = sqrt(pow(p.x - c.x, 2) + pow(p.y - c.y, 2));
	double azimut = get_azimut(c, p);

	return angle_point(c, rad, azimut / (2 * pi) + angle / 360);
}

//----------------------------------------------------------------------------

void Plane_engine::fly()
{
	int dy = k * dx;
	bort->move(dx, dy);

	bool new_course{ false };
	Point min = bort->min();
	Point max = bort->max();

	if (xm - 10 < max.x || min.x < 0) {
		dx *= -1;
		set_course();
	}

	if (ym - 10 < max.y || min.y < 0) {
		set_course();
	}
}

void Plane_engine::set_course()
{
	constexpr int maxspeed{8}, minspeed{2};
	double a = randint(1, 20) * 1.0 / 100 + 1;
	double b = randint(1, 20) * 1.0 / 100 + 1;

	static bool kk{true};
	if (abs(k * dx) < minspeed) kk = true;
	if (abs(k * dx) > maxspeed) kk = false;
	k *= -1 * (kk ? a : 1.0 / a);

	static bool dxx{true};
	if (abs(dx) < minspeed) dxx = true;
	if (abs(dx) > maxspeed) dxx = false;
	dx *= (dxx ? b : 1.0 / b);

	set_azimut();
	fix_position();
}

void Plane_engine::set_azimut()
{
	double aznew = get_azimut({0, 0}, {int(dx * 10), int(k * dx * 10)}) * 180 / pi;
	if (!phisics) {
		bort->init(bort->center());
	}
	bort->rotate(aznew - bort->azimut());
	bort->set_azimut(aznew);
}

void Plane_engine::fix_position()
{
	int dx{ 0 }, dy{ 0 };
	Point min = bort->min();
	Point max = bort->max();

	if (xm - 10 < max.x) {
		dx = xm - 11 - max.x;
	}
	else if (min.x < 0) {
		dx = - min.x + 1;
	}

	if (ym - 10 < max.y) {
		dy = ym - 11 - max.y;
	}
	else if (min.y < 0) {
		dy = - min.y + 1;
	}

	bort->move(dx, dy);
}
//----------------------------------------------------------------------------
