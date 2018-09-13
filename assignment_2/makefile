torrent : misc_utils.o torrent_file.o start_app.o
	g++ -o torrent misc_utils.o torrent_file.o start_app.o -lcrypto

misc_utils.o : misc_utils.cpp
	g++ -c misc_utils.cpp -o misc_utils.o

torrent_file.o : torrent_file.cpp
	g++ -c torrent_file.cpp -o torrent_file.o

start_app.o : start_app.cpp
	g++ -c start_app.cpp -o start_app.o
