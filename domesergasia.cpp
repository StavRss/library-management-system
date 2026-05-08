#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
class CopyNode { 
// Κόμβος που αναπαριστά ένα αντίτυπο 
public:
    int copyID;          // μοναδικός αριθμός αντιτύπου
    string status;       // κατάσταση 
    CopyNode* next;      // δείκτης στο επόμενο αντίτυπο

    CopyNode(int id, string st){
        copyID = id;     // αποθήκευση ID
        status = st;     // αποθήκευση κατάστασης
        next = nullptr;  // αρχικά δεν δείχνει πουθενά (τέλος λίστας)
    }
};

class BookNode { 
// Κόμβος που αναπαριστά ένα βιβλίο
public:
    string title;        // τίτλος
    string author;       // συγγραφέας
    string ISBN;         // ISBN 

    CopyNode* copiesHead; // δείκτης στην αρχή της λίστας 
    BookNode* next;       // δείκτης στο επόμενο βιβλίο

    BookNode(string t, string a, string i){
        title = t;        // αποθήκευση τίτλου
        author = a;       // αποθήκευση συγγραφέα
        ISBN = i;         // αποθήκευση ISBN
        copiesHead = nullptr; // αρχικά δεν υπάρχουν αντίτυπα
        next = nullptr;       // δεν υπάρχει επόμενο βιβλίο
    }

    void addCopy(int copyId , string status="available"); // προσθήκη αντιτύπου
    bool removeCopy(int copyId);   // διαγραφή αντιτύπου
    bool borrowCopy(int copyId);   // δανεισμός
    bool returnCopy(int copyId, bool isDamaged = false);   // επιστροφή
    int countAvailable();          // αριθμός διαθέσιμων
};

class Library{
public:
    BookNode* booksHead;  // δείκτης στην αρχή της λίστας βιβλίων

    Library(){
        booksHead = nullptr; // αρχικά δεν υπάρχουν βιβλία
    }

    void addBook(string title, string author,string isbn); // προσθήκη βιβλίου
    bool removeBook(string isbn); // διαγραφή βιβλίου
    BookNode* findBook(string isbn); // αναζήτηση βιβλίου
    void printAll();              // εκτύπωση όλων
    void loadFromFile(const std::string& filename); // φόρτωση από αρχείο
    void saveToFile(string filename);   // αποθήκευση σε αρχείο
    ~Library();                  // destructor 
};

void BookNode::addCopy(int copyId , string status){
    CopyNode* newNode = new CopyNode(copyId, status); 
    // δημιουργεί νέο κόμβο αντιτύπου στη μνήμη

    if (copiesHead == nullptr || copyId < copiesHead->copyID) {
        // αν η λίστα αντιτύπων είναι άδεια ή αν το καινούριο αντίτυπο έχει μικρότερο ID και πρέπει να μπεί πρώτο

        newNode->next = copiesHead;
        copiesHead = newNode;
        // ο νέος κόμβος γίνεται ο πρώτος της λίστας

    } else { 
        // αν υπάρχουν ήδη αντίτυπα

        CopyNode* temp = copiesHead;
        // ξεκινάμε από την αρχή της λίστας

         while (temp->next != nullptr && temp->next->copyID < copyId) {
            // όσο υπάρχει επόμενος κόμβος

            temp = temp->next;
            // πήγαινε στον επόμενο κόμβο
        }

        newNode->next = temp->next;
        temp->next = newNode;
        // σύνδεσε τον τελευταίο κόμβο με τον νέο
    }
}

bool BookNode::removeCopy(int copyId){
    CopyNode* temp = copiesHead;   // ξεκινάω από την αρχή της λίστας
    CopyNode* prev = nullptr;      // προηγούμενος κόμβος (αρχικά δεν υπάρχει)

    while (temp != nullptr){       // όσο δεν έχω φτάσει στο τέλος της λίστας

        if (temp->copyID == copyId){   // αν βρήκα το αντίτυπο που θέλω

            if (prev == nullptr){      // αν είναι το πρώτο node
                copiesHead = temp->next;  // μετακινώ την αρχή της λίστας
                delete temp;              // διαγράφω τον κόμβο από τη μνήμη
                return true;             
            }
            else {                      // αν είναι στη μέση ή στο τέλος
                prev->next = temp->next; // ενώνω τη λίστα 
                delete temp;             // διαγράφω τον κόμβο
                return true;            
            }
        }

        prev = temp;        // μετακινώ τον prev στον τωρινό κόμβο
        temp = temp->next;  // πάω στον επόμενο κόμβο
    }

    return false;  // δεν βρέθηκε το copyId
}


bool BookNode::borrowCopy(int copyId){
    // δείκτης που ξεκινά από την αρχή της λίστας αντιτύπων
    CopyNode* temp = copiesHead;
    // όσο δεν έχουμε φτάσει στο τέλος της λίστας
    while (temp != nullptr) {
       
        if (temp->copyID == copyId) { // αν βρούμε το αντίτυπο
            
            if (temp->status == "available") {
                temp->status = "borrowed";
                return true; // βρέθηκε και δανείστηκε
            }
            
            return false; // βρέθηκε αλλά δεν ήταν διαθέσιμο
        }

        temp = temp->next; // πάμε στον επόμενο κόμβο
    }

    return false; // δεν βρέθηκε καθόλου το copyId
}

bool BookNode::returnCopy(int copyId, bool isDamaged){
    CopyNode* temp = copiesHead;   // ξεκινάω από την αρχή της λίστας

    while (temp != nullptr) {      // όσο υπάρχουν κόμβοι

        if (temp->copyID == copyId) {   // βρήκα το σωστό αντίτυπο

            if (temp->status == "borrowed") {   // πρέπει να είναι borrowed

                if (isDamaged){                 // αν είναι damaged
                    temp->status = "damaged";   // αλλάζω status
                    return true;                // επιτυχία
                }
                else{
                    temp->status = "available"; // αλλιώς γίνεται available
                    return true;                // επιτυχία
                }
            }


            return false; // βρέθηκε το copy αλλά δεν ήταν borrowed
        }

        temp = temp->next;   // πάω στο επόμενο node
    }

    return false; // δεν βρέθηκε καθόλου το copyId
}

int BookNode::countAvailable(){
    int count = 0; // μετράει διαθέσιμα αντίτυπα
    CopyNode* temp = copiesHead; // ξεκινάω από την αρχή της λίστας

    while (temp != nullptr) { // όσο υπάρχουν κόμβοι
        if (temp->status == "available") { // αν το αντίτυπο είναι διαθέσιμο
            count++; 
        }
        temp = temp->next; // πάω στον επόμενο κόμβο
    }

    return count; 
}

void Library::addBook(string title, string author,string isbn){
    BookNode* newNode = new BookNode(title, author, isbn);
    // δημιουργεί νέο κόμβο βιβλίου στη μνήμη

    if (booksHead == nullptr || title < booksHead->title) {
        // αν η λίστα βιβλίων είναι άδεια
        newNode->next=booksHead;
        booksHead = newNode;
        // ο νέος κόμβος γίνεται ο πρώτος της λίστας

    } else {
        // αν υπάρχουν ήδη βιβλία

        BookNode* temp = booksHead;
        // ξεκινάμε από την αρχή της λίστας

        while (temp->next != nullptr && temp->next->title< title) {
            // όσο υπάρχει επόμενος κόμβος

            temp = temp->next;
            // πήγαινε στον επόμενο κόμβο
        }
        newNode->next = temp->next;
        temp->next = newNode;
        // σύνδεσε τον τελευταίο κόμβο με τον νέο
    }
}

bool Library::removeBook(string isbn){
   //δείκτης για να διατρέξουμε τη λίστα 
    BookNode* temp = booksHead;

    // Δείκτης στον προηγούμενο κόμβο 
    BookNode* prev = nullptr;

    // Διατρέχουμε όλη τη λίστα βιβλίων
    while (temp != nullptr){

        // Αν βρούμε το βιβλίο 
        if (temp->ISBN == isbn){
            // Έλεγχος για borrowed
            // Ξεκινάμε από τη λίστα αντιτύπων του βιβλίου
            CopyNode* ctemp = temp->copiesHead;
            // Διατρέχουμε όλα τα αντίτυπα
            while (ctemp != nullptr){

                // Αν βρούμε δανεισμένο
                if (ctemp->status == "borrowed"){
                    return false; 
                    // Δεν επιτρέπεται να το διαγράψουμε
                }

                //επόμενο αντίτυπο
                ctemp = ctemp->next;
            }

           
            //Διαγραφή όλων των CopyNode

            // Ξεκινάμε πάλι από την αρχή της λίστας αντιτύπων
            ctemp = temp->copiesHead;

            // Διαγράφουμε  όλα τα αντίτυπα
            while (ctemp != nullptr){

                // Κρατάμε τον επόμενο κόμβο πριν σβήσουμε τον τωρινό
                CopyNode* nextNode = ctemp->next;

                // Διαγραφή από τη μνήμη
                delete ctemp;

                // Προχωράμε στον επόμενο
                ctemp = nextNode;
            }
            // Διαγραφή του BookNode
           // Αν το βιβλίο είναι το πρώτο στη λίστα
            if (prev == nullptr){
                // Μετακινούμε την αρχή της λίστας
                booksHead = temp->next;
            } 
            else {
                // ενώνουμε τη λίστα 
                prev->next = temp->next;
            }
            // Διαγράφουμε το ίδιο το βιβλίο
            delete temp;
            return true; // πέτυχε η διαγραφή
        }
        // πάμε στους επόμενους κόμβους
        prev = temp;        // ο τωρινός γίνεται προηγούμενος
        temp = temp->next; // πάμε στον επόμενο κόμβο
    }
    return false;
}


BookNode*  Library::findBook(string isbn){
    BookNode* temp = booksHead;
    // Διατρέχουμε όλη τη λίστα βιβλίων
    while (temp != nullptr){

        // Αν βρούμε το βιβλίο 
        if (temp->ISBN == isbn){
            return  temp;
        }
        temp = temp->next;
    }
//
        return nullptr; // δεν βρέθηκε το βιβλίο
    }


    void Library::printAll(){
    BookNode* temp = booksHead;
    // Διατρέχουμε όλη τη λίστα βιβλίων
    while (temp != nullptr){

        // Εκτύπωση 
        cout << "Title: " << temp->title << endl;
        cout << "Author: " << temp->author << endl;
        cout << "ISBN: " << temp->ISBN << endl;

        // Διατρέχουμε τη λίστα αντιτύπων για να εκτυπώσουμε κατάσταση 
        CopyNode* temp2 = temp->copiesHead;
        int c=0;
        while (temp2 != nullptr){
            c++;
            temp2 = temp2->next;
            

        }
        
        cout << "Copies: " << c << endl;
        cout << "Available Copies: " << temp->countAvailable() << endl;

        
        temp = temp->next; // πάμε στο επόμενο βιβλίο
    }

}

 //Διαχωρίζει μια γραμμή στα tokens της βάσει του διαχωριστικού delim.
std::vector<std::string> parseLine(const std::string& line, char delim = '|') {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Φορτώνει τη βιβλιοθήκη από αρχείο
void Library::loadFromFile(const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout <<  "sfalma: den htan dynato to anoigma tou arxeiou" 
                  << filename << "'\n";
        return;
    }
std::string line;

    // Δείκτης στο τρέχον βιβλίο
    BookNode* currentBook = nullptr;

    // Διαβάζουμε κάθε γραμμή
    while (std::getline(file, line)) {

        if (line.empty()) continue; // αγνόηση κενών γραμμών

        std::vector<std::string> tokens = parseLine(line);

       
        //BOOK
        if (tokens[0] == "BOOK") {
            // tokens[1] = ISBN
           // tokens[2] = τίτλος
          // tokens[3] = συγγραφέας 

            // εισαγωγή βιβλίου
            

            addBook(tokens[2], tokens[3], tokens[1]);
            currentBook = findBook(tokens[1]);
        }

        //COPY
        else if (tokens[0] == "COPY") {
            // tokens[1] = copyId (μετατροπή: std::stoi(tokens[1]))
           // tokens[2] = status

            if (currentBook != nullptr) {
                int id = stoi(tokens[1]);
                string status = tokens[2];

                currentBook->addCopy(id, status);
            }
        }

        //Άλλο
        else {
            std::cout << "agnwsth grafh " << tokens[0] << "\n";
        }
    }

    file.close();
}

void Library::saveToFile(string filename) {

    ofstream file(filename);
    //έλεγχος αν άνοιξε το αρχείο
    if (!file.is_open()) {
        cout << "sfalma: den htan dynato to anoigma tou arxeiou\n";
        return;
    }

    BookNode* tempBook = booksHead;

    while (tempBook != nullptr) {

        // γράφουμε βιβλίο
        file << "BOOK|" 
             << tempBook->ISBN << "|"
             << tempBook->title << "|"
             << tempBook->author << "\n";

        // αντίτυπα
        CopyNode* tempCopy = tempBook->copiesHead;

        while (tempCopy != nullptr) {

            file << "COPY|" 
                 << tempCopy->copyID << "|"
                 << tempCopy->status << "\n";

            tempCopy = tempCopy->next;
        }

        tempBook = tempBook->next;
    }
}
Library::~Library() {

    BookNode* tempBook = booksHead;

    while (tempBook != nullptr) {

        // διαγραφή copies
        CopyNode* tempCopy = tempBook->copiesHead;

        while (tempCopy != nullptr) {
            CopyNode* nextCopy = tempCopy->next;
            delete tempCopy;
            tempCopy = nextCopy;
        }

        // διαγραφή βιβλίου
        BookNode* nextBook = tempBook->next;
        delete tempBook;
        tempBook = nextBook;
    }

    booksHead = nullptr;
}

int main() {
    Library lib;

    int choice;
    //δημιουργία μενού  
    do {
        cout << "====== BIBLIOTHIKH KEDRIKO MENU ======\n";

        cout << "1. Fortwsh vivliothikhs apo arxeio\n";
        cout << "2. Apothikeush vivliothikhs se arxeio\n";
        cout << "3. Emfanish olon ton vivliwn\n";
        cout << "4. Anazitisi vivliou (me ISBN)\n";
        cout << "5. Prosthiki neou vivliou\n";
        cout << "6. Diagrafi vivliou\n";
        cout << "7. Prosthiki antitypou se vivlio\n";
        cout << "8. Diagrafi antitypou apo vivlio\n";
        cout << "9. Danismos aditupou \n";
        cout << "10. Epistrofi aditupou \n";
        cout << "11. Emfanish diathesimwn antitypwn vivliou\n";
        cout << "0. Exodos\n";

        cin >> choice;

        // 1. loadfromfile
        if (choice == 1) {
            lib.loadFromFile("library.txt");
            // φορτώνει τα δεδομένα
        }

        // 2. saveToFile
        else if (choice == 2) {
            lib.saveToFile("library.txt");
             // αποθηκεύει τα δεδομένα 
        }

        // 3. printAll
        else if (choice == 3) {
            lib.printAll();
            // τα εμφανίζει όλα 
        }

        // 4. findbook
        else if (choice == 4) {
            string isbn;
            cout << "Dwse ISBN: ";
            cin >> isbn;

            BookNode* book = lib.findBook(isbn);
             // ψάχνει το βιβλίο 

             if (book != nullptr) {
                 // αν βρέθηκε δείχνει στοιχεία
                cout << "Title: " << book->title << endl;
                cout << "Author: " << book->author << endl;
                cout << "ISBN: " << book->ISBN << endl;
                cout << "Available copies: " << book->countAvailable() << endl;
            }
            else {
                //αν δε βρέθηκε
                 cout << "Den vrethike vivlio\n";
           }
        }

        // 5. addbook
        else if (choice == 5) {
            string isbn, title, author;

            cout << "Dwse ISBN: ";
            cin >> isbn;

            if (lib.findBook(isbn) != nullptr) {
                //αν υπάρχει ήδη
                cout << "To vivlio yparxei idi\n";
            }
            else {
                 // παίρνει τίτλο και συγγραφέα
                cout << "Dwse titlo: ";
                cin.ignore();
                getline(cin, title);

                cout << "Dwse suggrafea: ";
                getline(cin, author);
                 // προσθέτει νέο βιβλίο
                lib.addBook(title, author, isbn);
            }
        }

        // 6. removebook
        else if (choice == 6) {
            string isbn;
            cout << "Dwse ISBN: ";
            cin >> isbn;

            if (lib.removeBook(isbn)) {
                 // αν διαγράφηκε 
                  cout << "To vivlio diagrafike\n";
             }           
             else {
                //αν όχι
                cout << "Den borei na diagraftei \n";
              }
        }

        // 7. addCopy
        else if (choice == 7) {
            string isbn;
            int id;
            string status;

            cout << "Dwse ISBN: ";
            cin >> isbn;
            // βρίσκει βιβλίο
            BookNode* book = lib.findBook(isbn);

            if (book == nullptr) {
                // αν δεν υπάρχει
                cout << "Den vrethike vivlio\n";
            }
            else {
                cout << "Dwse copyID: ";
                cin >> id;

                cout << "Dwse status: ";
                cin >> status;

                book->addCopy(id, status);// προσθέτει αντίτυπο στη λίστα
            }
        }

        // 8. removeCopy
        else if (choice == 8) {
            string isbn;
            int id;

            cout << "Dwse ISBN: ";
            cin >> isbn;
            // βρίσκει βιβλίο
            BookNode* book = lib.findBook(isbn);

            if (book == nullptr) {
                // αν δεν υπάρχει
                cout << "Den vrethike vivlio\n";
            }
            else {
                cout << "Dwse copyID: ";
                cin >> id;
                // δείκτης στην αρχή της λίστας
                CopyNode* temp = book->copiesHead;
                // διάσχιση λίστας
                while (temp != nullptr) {
                    // αν βρεθεί σταματάει
                    if (temp->copyID == id) break;
                    temp = temp->next;
                     // αλλιώς πάμε στο επόμενο node
                }

                if (temp == nullptr) {
                    // δεν υπάρχει
                    cout << "Den vrethike antitypo\n";
                }
                else if (temp->status == "borrowed") {
                    // δεν διαγράφεται
                    cout << "Den borei na diagraftei\n";
                }
                else {
                    book->removeCopy(id);
                    // διαγράφεται
                    cout << "To antitypo diagrafike\n";
                }
            }
        }

        // 9.borrowCopy  
        else if (choice == 9) {
            string isbn;
            int id;

            cout << "Dwse ISBN: ";
            cin >> isbn;
            // βρίσκει βιβλίο
            BookNode* book = lib.findBook(isbn);

            if (book == nullptr) {
                cout << "Το βιβλίο δεν βρέθηκε!\n";
            }
            else {
                cout << "Dwse copyID: ";
                cin >> id;
                // αρχή λίστας
                CopyNode* temp = book->copiesHead;

                while (temp != nullptr) {
                    // βρισκει το αντίτυπο αρα σταματάει 
                    if (temp->copyID == id)
                        break;
                    temp = temp->next;
                     // αλλιώς πάμε στο επόμενο node
                }

                if (temp == nullptr) {
                    cout << "Den vrethike antitypo\n";
                }
                else if (temp->status == "available") {
                    book->borrowCopy(id);
                     //status γίνεται borrowed
                    cout << "Daneistike\n";
                    //εμφανίζει διαθέσιμα
                    cout << book->countAvailable() << " διαθέσιμα\n";
                }
                else {
                    cout << "Den einai diathesimo\n";
                }
            }
        }

        // 10. returnCopy
        else if (choice == 10) {
            string isbn;
            int id;
            string damage;

            cout << "Dwse ISBN: ";
            cin >> isbn;
            // βρίσκει βιβλίο
            BookNode* book = lib.findBook(isbn);

            if (book == nullptr) {
                cout << "Den vrethike vivlio\n";
            }
            else {
                cout << "Dwse copyID: ";
                cin >> id;
                // αρχή λίστας
                CopyNode* temp = book->copiesHead;

                while (temp != nullptr) {
                    // βρίσκει αντίτυπο αρα σταματάει
                    if (temp->copyID == id)
                        break;
                    temp = temp->next;
                     // αλλιώς πάμε στο επόμενο node
                }

                if (temp == nullptr) {
                    cout << "Den vrethike antitypo\n";
                }
                else if (temp->status == "available") {
                    cout << "To antitypo den einai daneismeno\n";
                }
                else {
                    cout << "Katastasi : ";
                    cin >> damage;

                    bool isDamaged = (damage == "damaged");
                    // αν γράψει damaged είναιtrue
                   // αν γράψει ok είναι  false

                    book->returnCopy(id, isDamaged);
                    //αν isDamaged = true το κάνει damaged αλλιώς το κάνει available

                    cout << "To antitypo epistrafike\n";
                }
            }
        }

        // 11. countAvailable
        else if (choice == 11) {
            string isbn;

            cout << "Dwse ISBN: ";
            cin >> isbn;
            // βρίσκει βιβλίο
            BookNode* book = lib.findBook(isbn);

            if (book != nullptr) {
                int count = book->countAvailable();// μετράει available
                cout << "diathesima adutipa " << count << endl;
            }
            else {
                cout << "Den vrethike vivlio\n";
            }
        }

    } while (choice != 0);

    return 0;
}