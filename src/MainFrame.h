#pragma once

#include <ImGuiPack.h>

class MainFrame {
public:
	bool init();
	void unit();
	void display(const ImVec2& vPos, const ImVec2& vSize);

public:
	static MainFrame* Instance() {
		static MainFrame _instance;
		return &_instance;
	}
};
