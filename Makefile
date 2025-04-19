Sources := main.cpp gui_main.cpp widgets.cpp
Sources += imgui/imgui.cpp \
		   imgui/imgui_draw.cpp imgui/imgui_widgets.cpp \
		   imgui/misc/cpp/imgui_stdlib.cpp \
		   imgui/examples/imgui_impl_glfw.cpp \
		   imgui/examples/imgui_impl_opengl2.cpp

Includes  := imgui imgui/examples imgui/misc/cpp

Libraries := glfw GL

BuildDir := build


Objects  := $(Sources:%.cpp=%.o)

CXXFLAGS := $(Includes:%=-I%) -g
LDLIBS   := $(Libraries:%=-l%)

CXXFLAGS += --std=c++17
CXXFLAGS += -Werror
CXXFLAGS += -Wformat -Wformat-security 


Dear_Board: $(Objects:%=$(BuildDir)/%)
	$(CXX) $(Objects:%=$(BuildDir)/%) $(LDLIBS) -o $@

$(BuildDir)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BuildDir)

