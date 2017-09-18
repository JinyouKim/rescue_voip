rescue : voice_server.o thread_session.o main.o voice_recorder.o voice_encoder.o voice_player.o voice_decoder.o stream_info.o
	g++ -g -ggdb -std=c++11 voice_server.o thread_session.o main.o voice_decoder.o voice_player.o voice_recorder.o voice_encoder.o stream_info.o -o rescue -I/usr/local/include/jrtplib3 -ljrtp -ljthread -lpthread -lasound -lopus -lboost_thread -lboost_system -lrt

stream_info.o : stream_info.cpp voice_player.o
	g++ -std=c++11 -c stream_info.cpp -o stream_info.o

voice_decoder.o : voice_decoder.cpp
	g++ -std=c++11 -c voice_decoder.cpp -o voice_decoder.o

voice_player.o : voice_player.cpp voice_decoder.o
	g++ -std=c++11 -c voice_player.cpp -o voice_player.o

voice_encoder.o : voice_encoder.cpp
	g++ -std=c++11 -c voice_encoder.cpp -o voice_encoder.o

voice_recorder.o : voice_recorder.cpp
	g++ -std=c++11 -c voice_recorder.cpp -o voice_recorder.o

voice_server.o : voice_server.cpp voice_encoder.o voice_recorder.o
	g++ -std=c++11 -c voice_server.cpp -o voice_server.o -I/usr/local/include/jrtplib3 

thread_session.o : thread_session.cpp voice_decoder.o voice_player.o
	g++ -std=c++11 -c thread_session.cpp -o thread_session.o -I/usr/local/include/jrtplib3
main.o : main.cpp
	g++ -std=c++11 -c main.cpp -o main.o -I/usr/local/include/jrtplib3
	

