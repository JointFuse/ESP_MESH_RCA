#include <winsock2.h>
#include <stdexcept>
#include <iostream>
#include <Windows.h>
#include <winuser.h>
#include "TextGraph.h"

using namespace std;

int main()
try{
	int x_center{ 0 };
	int y_center{ 0 };

	DEVMODEW dispsett;
	dispsett.dmSize = sizeof(DEVMODE);
	dispsett.dmDriverExtra = 0;

	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dispsett))
	{
		x_center = dispsett.dmPelsWidth / 2;
		y_center = dispsett.dmPelsHeight / 2;
	}

	TextGraph_lib::ConnectHub tiwin({ x_center - TextGraph_lib::Sizes::width / 2,
									  y_center - TextGraph_lib::Sizes::height / 2 });

	return Graph_lib::gui_main();
}
catch(runtime_error& err) {
	cout << "Runtime error: " << err.what() << endl;
	return 1;
}
catch (exception& e) {
	 cout << "Exception: " << e.what() << endl;
	 return 2;
}