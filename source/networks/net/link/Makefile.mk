CXXFLAGS	+= -I./source/networks/net/link

VPATH += source/networks/net/link

OBJ += source/networks/net/link/link.o
OBJ += source/networks/net/link/link_mac.o
OBJ += source/networks/net/link/link_phy.o
OBJ += source/networks/net/link/link_data.o
