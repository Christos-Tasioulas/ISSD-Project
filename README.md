# ISSD-Project

## Συγγραφείς:
Γεώργιος-Σάββας Δημητρίου 1800045, Ραφαήλ-Χρήστος Τασιούλας 1800191, Κυριαζής Χατζηγιαννάκης 1800211

## Εργασία για το Project Ανάπτυξη Λογισμικού για Πληροφοριακά Συστήματα.

### 1ο Παραδοτέο:
Υλοποιήθηκε ο αλγόριθμος Partition Hash Join για πίνακες μίας στήλης και n-σειρών με στοιχεία ακέραιους. 
Στο πρόγραμμα δίνονται ως είσοδος δύο σχεσιακοί πίνακες με τα παραπάνω χαρακτηριστικά και εκτελεί την πράξη JOIN με τον παραπάνω αλγόριθμο μεταξύ των δύο πινάκων.
Υπάρχουν τα build directories με τα Makefile για το κύριο πρόγραμμα και το test πηγαίο αντίστοιχα,  
το source στο οποίο υπάρχουν όλα τα πηγαία αρχεία σε C++ και ένα config.txt όπου ο χρήστης ρυθμίζει τις παραμέτρους του προγράμματος.  

### 2ο Παραδοτέο:
Αρχικά, το make.sh μετατράπηκε σε Makefile για ευκολία χρήσης. Υλοποιήθηκαν οι συναρτήσεις του FileReader που διαβάζουν την είσοδο και τα queries. Επίσης υλοποιήθηκαν οι συναρτήσεις που "κατακερματίζουν" τα queries, τα ομαδοποιούν σε batches και απαντάνε στα ερωτήματά τους. Ακόμη εισάγαμε τις ενδιάμεσες σχέσεις των JOIN. Τέλος, υλοποιήσαμε και τα tests (συμπεριλαμβανομένου και του harness) για τις περισσότερες συναρτήσεις του κώδικά μας.

### Makefile
Το κεντρικό Makefile που περιέχει εντολές για το τρέξιμο του κώδικα και διάφορων άλλων λειτουργιών της εργασίας και εκτελέιται χωρίς την αλλαγή directory. Οι εντολές που υποστηρίζονται είναι οι εξής:
- make (test/all/harness)          (μεταγλώττιση)
- make run(_test/all/harness)      (εκτέλεση)
- make clean(_test/all/harness)    (αφαίρεση)
- make valgrind(_test/all/harness) (εκτέλεση με valgrind)
- make count(_test/all/harness)    (μέτρηση λέξεων και ποσότητάς τους σε κάθε αρχείο)
- make list(_test/all/harness)     (καταγραφή αρχείων)

Άμα θέλουμε να εκτελέσουμε εντολές για όλα τα εκτελέσιμα αρκεί να γράψουμε την εντολή που θέλουμε και μετά να γράψουμε all (πχ. make all, make run_all κ.ο.κ). Το ίδιο ισχύει και για το test αρχείο αποκλειστικά (make test, make run_test). 

### config.txt
Δίνει την ελευθερία στον χρήστη να αλλάξει τις εξής παραμέτρους:
- Τον αριθμό των λιγότερο σημαντικών bits που θα χρησιμοποιούνται για το hashing των αριθμών που δίνονται ως input.
- Αν το πρόγραμμα θα εμφανίζει τις σχέσεις που δόθηκαν στην είσοδο.
- Αν το πρόγραμμα θα εμφανίζει τους βοηθητικούς πίνακες (βλ. παρακάτω).
- Αν το πρόγραμμα θα εμφανίζει τo hash table στο οποίο γίνεται ο αλγόριθμος.
- Αν το πρόγραμμα θα εμφανίζει τα βαθύτερα partitions αν υπάρχουν.
- Αν το πρόγραμμα θα εμφανίζει τους κουβάδες από το hashing ξεχωριστά (βλ. παρακάτω).
- Αν το πρόγραμμα θα εμφανίζει το τελικό αποτέλεσμα.
- Τον αριθμό των hash κουβάδων.
- Το μέγεθος του bitmap.
- Αν θα γίνεται rehash.
- Τον load factor του hash table.
- Το ποσοστό της cache που θα χρησιμοποιηθεί για την εκτέλεση του αλγορίθμου.
- Το πόσα επίπεδα partition θα γίνονται. 

### build
Περιλαμβάνει το Makefile το οποίο περιλαμβάνει την make για μεταγλώττιση των αρχείων, την run για εκτέλεση και την run_valgrind για εκτέλεση του προγράμματος με valgrind. 
Περιλαμβάνει επίσης την make clean που διαγράφει όλα τα αρχεία που δημιουργήθηκαν από την μεταγλώττιση των πηγαίων αρχείων,
την make count που μετράει τις λέξεις και την ποσότητά τους σε κάθε πηγαίο αρχείο, την make list που καταγράφει τα header και πηγαία αρχεία και την make ls που είναι βοηθητική στην make list.
Περιλαμβάνει και το harness.cpp απο το submission του ACM Sigmod 2018 που εκτελεί το harness test για το πρόγραμμα.

### test_build
Ίδιο με το build αλλά περιλαμβάνει το αντίστοιχο Makefile για το test εκτελέσιμο.

### source
Υπάρχουν αρχέια .cpp/.h συνονόματα με τα αντίστοιχα directories. Bitmap, FileReader, HashTable (μαζί με HashTableEntry), List, PartitionedHashJoin, Relation, RowIdPair, RowIdRelation, Tuple.
Επίσης υπάρχει ο φάκελος Main με την main συνάρτηση.

#### Bitmap.cpp/.h
Ένας πίνακας που παίρνει τιμές 0 και 1 που αντιπροσωπέυει έναν αριθμό bits και αν είναι δεσμευμένα ή όχι. Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά του.

#### FileReader.cpp/.h
Διαβάζει την είσοδο που έχει δοθεί και δημιουργεί και τους αντίστοιχους πίνακες. Επίσης, θα διαβάζει τις παραμέτρους που δόθηκαν στο config.txt. Εναλλακτικά, αν δε δοθεί αρχείο με δεδομένα, έχουν γίνει hardcoded κάποιοι πίνακες για να δούμε πώς δουλεύει το πρόγραμμα. Τέλος, διαβάζει τα queries από το small.work, τα χωρίζει σε batches και τις εποστρέφει για να εκτελεστούν. 

#### HashTable.cpp/.h
Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά που αφορούν τους πίνακες κατακερματισμού, το rehashing, τις συναρτήσεις αναζήτησης.

#### HashTableEntry.cpp/.h
Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά που αφορούν το κάθε στοιχείο στα HashTable που υλοποιήθηκαν.   

#### IntermediateArray.cpp/.h
Περιλαμβάνει τις ενδιάμεσες σχέσεις τα αποτελέσματα των μεταξύ τους JOIN και μέλη του IntermediateRepresentation (βλ. παρακάτω) όπως όλους τους πίνακες δεδομένων, και τα configurations του αλγορίθμου PartitionedHashJoin. Καλεί τις JOIN για εξειδικευμένες περιπτώσεις queries είτε μεταξύ δύο σχέσεων από ίδιους ή διαφορετικούς IntermediateArrays είτε μεταξύ φίλτρου αλλά και τον υπολογισμό αθροίσματος των στοιχείων μιας στήλης. Περιαμβάνεται σε λίστα του IntermediateRepresentation.

#### IntermediateRelation.cpp/.h
Περιλαμβάνει την ομώνυμη δομή με το όνομα μιας σχέσης r0, r1 etc. και το πως την κάνει label το FROM κομμάτι του query.Περιαμβάνεται σε λίστα του IntermediateArray. 

#### IntermediateRepresentation.cpp/.h
Περιλαμβάνει μία λίστα με όλες τις ενδιάμεσες αναπαραστάσεις από το τρέχον query, όλους τους πίνακες δεδομένων, και τα configurations του αλγορίθμου PartitionedHashJoin. Καλεί γενικευμένα την Join τα φίλτρα και τα αθροίσματα στηλών.

#### List.cpp/.h
Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά που αφορούν μια λίστα

#### Main.cpp
Από εκέι ξεκινάει η εκτέλεση του PartitionedHashJoin. 

#### PartitionedHashJoin.cpp/.h
Εκεί εκτελούνται τα κύρια κομμάτια του αλγορίθμου. 
Έχει τη μορφή κλάσης που αποτελείται από τις 2 σχέσεις που δίνονται, τον αριθμό των λιγότερο σημαντικών bits κάθε στοιχείου για hashing και αν χρειάζεται να γίνει 2ο hashing στην JOIN. Υλοποιούνται οι συναρτήσεις για τον αλγόριθμο και συναρτήσεις εμφάνισεις των αποτελεσμάτων και των ενδιάμεσων βημάτων. 

#### PartitionedHashJoinInput.cpp/.h
Περιλαμβάνει σε μία κλάση όλα τα configurations για τον αλγόριθμο της Partitioned Hash Join από το config.txt.

#### PredicatesParser.cpp/.h
Περιέχει τις συναρτήσεις που αφορούν το τι πράξη θα γίνει μεταξύ των πινάκων που δίνονται, αν θα έχουμε join ή filtering μεταξύ τους ή με κάποια μεταβλητή αντίστοιχα. Χωρίζει το 2ο κομμάτι του query σε κατηγορήματα.

#### ProjectionsParser.cpp/h
Κάνει parsing στο τρίτο κομμάτι του Query το οποίο περιλαμβάνει τις σχέσεις και τις στήλες τους των οποίων ζητείται το άθροισμα.

#### Query.cpp/.h
Εδώ βρίσκεται όλη η πληροφορία ενός query, διαχωρισμένα σε relations, predicates και projections

#### QueryHandler.cpp/.h
Αποθηκεύονται οι σχέσεις και τα batches των Queries. Επίσης υπάρχουν οι συναρτήσεις που επιστρέφουν τις απαντήσεις στα queries. Εδώ γίνονται οι κατάλληλες κλήσεις για την εκτέλεση των queries

#### Relation.cpp/.h
Μία σχέση με n σειρές, που θα δίνεται στην αρχή/είσοδο του προγράμματος και αποτελείται απο Tuples (αριθμός σειράς, user data).
Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά που την αφορούν

#### RowIdPair.cpp/.h
Είναι στοιχείο του RowIdRelation(βλ. παρακάτω) και είναι ένα ζευγάρι δύο σειρών, μία από κάθε σχέση που δόθηκε στην είσοδο πιο συγκεκριμένα το id κάθε σειράς. 

#### RowIdRelation.cpp/.h
Είναι μία σχέση που αποτελείται από ένα ζευγάρι δύο σειρών. Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά που την αφορούν.

#### Test.cpp/ acutest.h
Το test πηγαίο αρχείο που υλοποιούνται όλα τα test για την PartitionedHashJoin με την βοήθεια του acutest framework. Δημιουργήθηκε test για ολόκληρο τον αλγόριθμο του Partitioned Hash Join διότι γίνεται όλος σε μία μέθοδο. Το test όμως για αυτόν είναι μικρής κλίμακας διότι δεν είναι εύκολο να απομιμηθεί ένα μεγάλο αποτέλεσμα του αλγορίθμου για τον έλεγχό του. Δημιουργήθηκαν επίσης tests για τα αντικείμενα BitMap, FileReader, HashTable, IntermediateArray, List και Query διότι αυτά τα αρχεία έχουν μεθόδους με περιπτώσεις που δεν είναι setters, getters, prints και ελέγχοι καταστάσεων. Για όλες τις άλλες συναρτήσεις σε αυτά τα αρχεία υπάρχουν tests σε αυτό το .cpp αρχείο. 
Σημείωση 1: Δεν έγινε test για την executeJoin και την executeFilter του IntermediateRepresentation διότι δεν υπάρχει διαφορετικός τρόπος να επιστραφούν τα αποτελέσματα των πράξεων στη δομή από αυτόν που γίνεται στο IntermediateArray όπου έχουν ελεγχθεί ξεχωριστά όλες οι υποπεριπτώσεις. 
Σημείωση 2: Δεν

#### Tuple.cpp/.h
Είναι ένα στοιχέιο του Relation.
Περιλαμβάνει τις συναρτήσεις και τα χαρακτηριστικά που αφορούν τα Tuples(παρόμοια με αυτά της Python) 
με το ένα μέλος να είναι το id μίας σειράς και το άλλο να είναι το στοιχείο μίας σειράς(user data).
Το user data είναι τύπου void* με σκοπό την γενίκευση της δομής δεδομένων, ώστε να είναι πιο ευέλικτο για άλλες χρήσεις. 
Καταναλώνει παραπάνω χώρο μνήμης λόγω της ευθυγράμμισης κάθε tuple, αλλά έτσι είναι πιο πιθανό να εφαρμοστούν όλες οι περιπτώσεις του αλγορίθμου.



