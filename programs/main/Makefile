# Λίστα με objects (.o) για κάθε εκτελέσιμο που θέλουμε να φτιάξουμε.
#
# Για το ADTVector χρησιμοποιούμε την υλοποίηση του k08.a library.
#
main = Main.o $(LIB)/ISSD_Project1.a

ARGS = 

# Ο βασικός κορμός του Makefile
include ../../common.mk

# Για να φτιάξουμε το k08.a, τρέχουμε το make στο lib directory.
# Αυτό γίνεται ακόμα και αν υπάρχει ήδη το k08.a (.PHONY), γιατί ίσως πρέπει να ενημερωθεί.
.PHONY: $(LIB)/k08.a
$(LIB)/k08.a:
	$(MAKE) -C $(LIB) k08.a