# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

# 대상 실행 파일 이름
TARGET = btree_test

# 소스 파일 및 헤더 파일
SRCS = btree_test.cpp
HDRS = btree.hpp

# 오브젝트 파일 생성
OBJS = $(SRCS:.cpp=.o)

# 기본 목표
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 개별 소스 파일 컴파일
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 클린업
clean:
	rm -f $(OBJS) $(TARGET)

# 실행
run: $(TARGET)
	./$(TARGET)
