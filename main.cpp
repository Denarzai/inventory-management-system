/*
 * ============================================================================
 *  Inventory & E-Commerce Management System (Console, C++)
 * ============================================================================
 *  A role-based store management system with three user types:
 *    - Admin    : user/role management, product management, announcements,
 *                 activity logs, sales & revenue analysis, audit trail
 *    - Employee : support requests, inventory management, feedback, announcements
 *    - Customer : catalog browsing & filtering, cart, checkout with discount
 *                 codes, wishlist, order history, feedback, support requests
 *
 *  Data is persisted in plain-text files (Catalog.txt, Cart.txt,
 *  verification.txt, orderhis.txt, wishlist.txt, etc.) that act as a simple
 *  flat-file database. String handling and comparison are implemented manually
 *  (arrayComparison, manualStrcpy) instead of using the C++ string library.
 *
 *  Build:  g++ -std=c++11 -o store main.cpp
 *  Run:    ./store
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <iomanip> // For setw formatting
using namespace std;

// Function Declarations
void customer(void);
void admin(void);
void employee(void);
bool searchid(const char a[], const char b[], const char c[]);
bool searchchr(const char a[], const char b[]);
bool arrayComparison(const char arr1[], const char arr2[]);
void manualStrcpy(char destination[], const char source[]);
void cleanField(char s[]);
void changecatalog(const char a[], int newQuantity);
bool catalogcheck(const char a[],int b);
bool ifincatalog(const char a[]);
bool inwishlist(const char a[]);
void storeActivity(char username[], const char action[]);
void viewActivities();
void storeAudit(const char admin[], const char action[]);
void viewAudit();
void addAnnouncement(const char name[], const char message[]);
void deleteAnnouncement(const char name[]);
void viewannouncement();
void alerts();
void inventorymanage(char[]);
void updateQuantity(char name[], int newQuantity);
void deleteProduct(char name[]);
void addProduct(const char name[], int price, int stockQuantity, const char category[], int productId);

// Global Variables
char name[100];
char password[100];
char role[20];
int Price;
int StockQuantity;
char Category[100];
char ProductId[100];
char enteredUsername[100];
char fileUsername[100];



// Main function
int main() {
    char Role;
    // Role selection menu
    cout << "1) Admin" << endl;
    cout << "2) Employee" << endl;
    cout << "3) Customer" << endl;
    cout << "Enter your Role: ";
    cin >> Role;
    cin.ignore(); // Clear input buffer

    // Calls the appropriate function based on role
    switch (Role) {
        case '1':
            admin();
            break;
        case '2':
            employee();
            break;
        case '3':
            customer();
            break;
        default:
            cout << "Invalid Role Selected!" << endl;
    }

    return 0;
}

void manualStrcpy(char destination[], const char source[]) {
    while (*source != '\0') {
        *destination = *source; 
        destination++;
        source++;
    }
    *destination = '\0'; 
}

// Removes invisible characters that sneak in when a text file is saved on
// Windows: a leading UTF-8 BOM (bytes EF BB BF) and any trailing carriage
// return / spaces / tabs. Without this, "admin" read from a CRLF file is
// actually "admin\r" and string comparisons fail even though it looks correct.
void cleanField(char s[]) {
    // Strip a leading UTF-8 BOM if present (only ever on the first field of a file)
    unsigned char *u = (unsigned char *)s;
    if (u[0] == 0xEF && u[1] == 0xBB && u[2] == 0xBF) {
        int i = 0;
        while (s[i + 3] != '\0') { s[i] = s[i + 3]; i++; }
        s[i] = '\0';
    }
    // Find the end of the string
    int len = 0;
    while (s[len] != '\0') len++;
    // Trim trailing carriage return, newline, spaces and tabs
    while (len > 0 && (s[len - 1] == '\r' || s[len - 1] == '\n' || s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

// Customer functionality
void customer(void) {
    float total;
    char opt;
    char actcode[20];
    int percent;
    char empName[20], line[100];
    bool loginOk = false; // tracks whether the credentials matched this attempt
    do {
        total = 0.0;
        cout << "\n------------Notifications-------------" << endl;
        fstream o1("discount.txt",ios::in);
        cout<<"Promo-Codes"<<setw(15)<<"Percentage\n";
        while(o1>>actcode>>percent){
            cout<<actcode<<setw(15)<<percent<<endl;
        }
        cout<<endl;
        o1.close();
        
        cout << "\nAnnouncements\n";
        cout << "-------------\n";
        cout<<left<< setw(20) << "Employee"  << setw(100) << "Message"<<endl << endl;
        o1.open("announcement.txt",ios::in);
        while(o1.getline(empName, 20, '|') && o1.getline(line,100,'\n')){
            cout<<left<< setw(20) << empName  << setw(60) << line <<endl;
        }
        cout<<endl;
        


        cout << "1) Sign up\n2) Login\n";
        cout << "Option: ";
        cin >> opt;
        cin.ignore(); // Clear input buffer

        if (opt == '1') {
            // Registering a new customer
            cout << "Enter Name: ";
            cin.getline(name, 100);
            cout << "Enter Password: ";
            cin.getline(password, 100);

            // Save user credentials in the verification file
            fstream id("verification.txt", ios::out | ios::app);
            id << name << "," << password << "," << "customer" << "\n";
            id.close();
            cout << "ID Registered Successfully.\n\n";
        }

        // Login functionality
        cout << "Login:\n";
        cout << "Enter Username: ";
        cin.getline(enteredUsername, 100);
        cout << "Enter Password: ";
        cin.getline(password, 100);
        cout << endl;

        // Check credentials once and reuse the result (avoids re-reading the file 3 times)
        loginOk = searchid(enteredUsername, password, "customer");
        if (loginOk) {
            cout << "Login Successful" << endl;
            storeActivity(enteredUsername, "Login Success");
        } else {
            cout << "Invalid ID or Password" << endl;
            storeActivity(enteredUsername, "Failed Login");
        }
    } while (!loginOk);

    // Customer menu
    do {
        cout << "\n----Customer Menu----\n";
        cout << "1) View catalog\n";
        cout << "2) Manage Cart\n";
        cout << "3) Checkout \n";
        cout << "4) Wishlist \n";
        cout << "5) View Order History and Give Feedback\n";
        cout << "6) Support Request\n";
        cout << "7) Logout\n";
        cout << "Option: ";
        cin >> opt;
        cin.ignore();

        switch (opt) {
            case '1': {
                // Display catalog
                fstream catalog("Catalog.txt", ios::in);

                cout << "\nCatalog:\n";
                cout << setw(15) << "Name" << setw(10) << "Price" << setw(10)
                     << "Stock" << setw(15) << "Category" << setw(15) << "Product ID" << endl;

                // Read and display catalog content
                while (catalog >> name >> Price >> StockQuantity >> Category >> ProductId) {
                    cout << setw(15) << name << setw(10) << Price << setw(10)
                         << StockQuantity << setw(15) << Category << setw(15) << ProductId << endl;
                }
                catalog.close();
                char filterOpt;
                do{
                cout<<endl;
                cout << "1) Name\n2) Price Range\n3) Category\n4) Product ID\n5) Go back\n";
                cout << "\nTo Filter Items Choose an Option: ";
                cin >> filterOpt;
                cin.ignore();

                fstream catalogFilter("Catalog.txt", ios::in);

                char name1[50], Category1[50], Productid[50];
                int price1, price2;

                switch (filterOpt) {
                    case '1':
                        cout << "Enter Name of Product: ";
                        cin.getline(name1, 50);
                        cout<<endl;
                        while (catalogFilter >> name >> Price >> StockQuantity >> Category >> ProductId) {
                            if (arrayComparison(name, name1)) {
                                cout << setw(15) << name << setw(10) << Price << setw(10)
                                     << StockQuantity << setw(15) << Category << setw(10) << ProductId << endl;
                            }
                        }
                        break;

                    case '2':
                        cout << "Enter Min Price Range: ";
                        cin >> price1;
                        cout << "Enter Max Price Range: ";
                        cin >> price2;
                        cout<<endl;
                        while (catalogFilter >> name >> Price >> StockQuantity >> Category >> ProductId) {
                            if (Price >= price1 && Price <= price2) {
                                cout << setw(15) << name << setw(10) << Price << setw(10)
                                     << StockQuantity << setw(15) << Category << setw(10) << ProductId << endl;
                            }
                        }
                        break;

                    case '3':
                        cout << "Enter Category of Product: ";
                        cin.getline(Category1, 50);
                        cout<<endl;
                        while (catalogFilter >> name >> Price >> StockQuantity >> Category >> ProductId) {
                            if (arrayComparison(Category, Category1)) {
                                cout << setw(15) << name << setw(10) << Price << setw(10)
                                     << StockQuantity << setw(15) << Category << setw(10) << ProductId << endl;
                            }
                        }
                        break;

                    case '4':
                        cout << "Enter Product ID: ";
                        cin.getline(Productid, 50);
                        cout<<endl;
                        while (catalogFilter >> name >> Price >> StockQuantity >> Category >> ProductId) {
                            if (arrayComparison(ProductId, Productid)) {
                                cout << setw(15) << name << setw(10) << Price << setw(10)
                                     << StockQuantity << setw(15) << Category << setw(10) << ProductId << endl;
                            }
                        }
                        break;
                    case '5':
                        cout<< "Going back...\n";
                        break;

                    default:
                        cout << "Invalid Input!" << endl;
                        break;
                }
                catalogFilter.close();
                }while(filterOpt!='5');
                break;
            }            

            case '2': {
                // Cart Management Menu
                do {
                    cout << "\n------Manage Cart------\n";
                    cout << "1) Add items\n";
                    cout << "2) Remove items\n";
                    cout << "3) Update item quantity\n";
                    cout << "4) View Cart\n";
                    cout << "5) Go back\n";
                    cout << "Enter your choice: ";
                    cin >> opt;
                    cin.ignore(); // Clear input buffer

                    switch (opt) {
                         case '1': { // Add item to cart
                            char productName[100];
                            int quantity;

                            cout << "\n------------Item Addition-------------------\n";
                            cout << "Enter Name of product: ";
                            cin>>productName;
                            cout << "Enter Quantity of Product: ";
                            cin >> quantity;
                            cin.ignore(); // Clear input buffer

                            // Check if product is already in the cart
                            if (!searchchr(productName,enteredUsername)) {

                                if(catalogcheck(productName,quantity)){
                                            fstream cart("Cart.txt", ios::app);
                                            cart << enteredUsername << " " <<name << " " << Price << " " << quantity << " " << Category << " " << ProductId << endl;
                                            cart.close();
                                            cout << "Item added to cart successfully!\n";
                                }
                            } else {
                                cout << "Product already exists in the cart.\n";
                            }
                            break;
                        }


                        case '2': { // Remove item from cart
                            char productName[100];
                            cout << "\n--------------Item Removal-----------------\n";
                            cout << "Enter Name of product to remove: ";
                            cin.getline(productName, 100);

                            fstream cart("Cart.txt", ios::in);
                            fstream temp("Temp.txt", ios::out);

                            bool itemRemoved = false;
                            while (cart >> enteredUsername >> name >> Price >> StockQuantity >> Category >> ProductId) {
                                if (!arrayComparison(name, productName)) {
                                    temp << enteredUsername << " " <<name << " " << Price << " " << StockQuantity << " " << Category << " " << ProductId << endl;
                                } else {
                                    itemRemoved = true;
                                }
                            }
                            cart.close();
                            temp.close();

                            // Replace original file with updated file
                            remove("Cart.txt");
                            rename("Temp.txt", "Cart.txt");

                            if (itemRemoved) {
                                cout << "Product removed successfully!\n";
                            } else {
                                cout << "Product not found in the cart.\n";
                            }
                            break;
                        }

                        case '3': { // Update item quantity in cart
                            char productName[100];
                            int newQuantity;
                            cout << "\n--------------Update Quantity----------------\n";
                            cout << "Enter Name of product to update: ";
                            cin.getline(productName, 100);
                            cout << "Enter new Quantity: ";
                            cin >> newQuantity;
                            cin.ignore(); // Clear input buffer

                            fstream cart("Cart.txt", ios::in);
                            fstream temp("Temp.txt", ios::out);
                            bool itemfound = false;
                            
                            while (cart >> fileUsername >> name >> Price >> StockQuantity >> Category >> ProductId) {
                                if (arrayComparison(name, productName) && arrayComparison(enteredUsername, fileUsername)) {
                                    itemfound= true;
                                    if(catalogcheck(productName,newQuantity)){
                                    temp << enteredUsername << " " <<name << " " << Price << " " << newQuantity << " " << Category << " " << ProductId << endl;
                                    
                                    }
                                    else{
                                        temp << enteredUsername << " " << name << " " << Price << " " << StockQuantity << " " << Category << " " << ProductId << endl;
                                    }
                                } else {
                                    temp << enteredUsername << " " << name << " " << Price << " " << StockQuantity << " " << Category << " " << ProductId << endl;
                                }
                            }
                            if(!itemfound){
                                    cout<<"Product is not in the cart.\n";
                                }
                            cart.close();
                            temp.close();

                            // Replace original file with updated file
                            remove("Cart.txt");
                            rename("Temp.txt", "Cart.txt");
                            break;
                        }



                        case '4': { // View cart
                            cout<<endl;
                            cout << "\n--------------------------Your Cart-----------------------\n";
                            fstream cart("Cart.txt", ios::in);
                            float total=0;

                            if (!cart) {
                                cout << "Your cart is empty!\n";
                            } else {
                                cout << setw(15) << "Name" << setw(10) << "Price" << setw(10)
                                     << "Quantity" << setw(15) << "Category" << setw(10) << "Product ID" << endl;

                                while (cart >> fileUsername >>name >> Price >> StockQuantity >> Category >> ProductId) {
                                    if(arrayComparison(fileUsername, enteredUsername)){                                    
                                        total = total + Price*StockQuantity;
                                        cout << setw(15) << name << setw(10) << Price << setw(10)<< StockQuantity << setw(15) << Category << setw(10) << ProductId << endl;
                                    }
                                    
                                }
                                cout<<"----------------------------------Total-------------------------------\n";
                                    cout<<"Total Price = "<<total<<endl<<endl;
                                cart.close();
                            }
                            break;
                        }

                        case '5':
                            cout << "Returning to the previous menu...\n";
                            break;

                        default:
                            cout << "Invalid choice!\n";
                            break;
                    }
                } while (opt != '5');
                break;
            }

            case '3':{
                //checkout of Final cart.
                float finaltotal=0;
                char opt;
                do{
                    cout<<"------------checkout----------\n";
                    cout<<"1) Review cart\n";
                    cout<<"2) Apply Discount Code\n";
                    cout<<"3) Confirm Purchase\n";
                    cout<<"4) Go back\n";
                    cout<<"Enter your choice: ";
                    cin>>opt;

                    switch(opt){
                        case '1':{ 
                                // View cart
                                cout << "\n--------------------------Your Cart-----------------------\n";
                                fstream cart("Cart.txt", ios::in);
                                float total=0;

                                if (!cart) {
                                    cout << "Your cart is empty!\n";
                                } else {
                                    cout << setw(15) << "Name" << setw(10) << "Price" << setw(10)
                                        << "Quantity" << setw(15) << "Category" << setw(10) << "Product ID" << endl;

                                    while (cart >> fileUsername >>name >> Price >> StockQuantity >> Category >> ProductId) {
                                        if(arrayComparison(fileUsername, enteredUsername)){                                    
                                            total = total + Price*StockQuantity;
                                            cout << setw(15) << name << setw(10) << Price << setw(10)<< StockQuantity << setw(15) << Category << setw(10) << ProductId << endl;
                                        }
                                        
                                    }
                                    if((finaltotal==0)){
                                        cout<<"----------------------------------Total-------------------------------\n";
                                        cout<<"Total Price = "<<total<<endl<<endl;
                                        } else {
                                            cout<<"----------------------------------Total-------------------------------\n";
                                            cout<<"Discounted Total Price = "<<finaltotal<<endl<<endl;
                                        }
                                        cart.close();
                                }
                                break;
                            }
                            

                        case '2':
                            {//discount code
                                char code[20];
                                cout<<"\n------Discount Code------\n";
                                cout<<"Enter Code: ";
                                cin>>code;
                                fstream o1("discount.txt",ios::in);
                                while(o1>>actcode>>percent){
                                    if(arrayComparison(actcode,code)){
                                    fstream cart("Cart.txt", ios::in);

                                    if (!cart) {
                                        cout << "Your cart is empty!\n";
                                    } 
                                    else {
                                        while (cart >> fileUsername >>name >> Price >> StockQuantity >> Category >> ProductId) {
                                            if(arrayComparison(enteredUsername, fileUsername) && arrayComparison(actcode, code)){
                                                total += Price*StockQuantity;
                                            
                                            }
                                        }
                                        finaltotal = (double)total-((double)total*((double)percent/100.0));
                                        cout << "Discounted Total = " << finaltotal<<endl;
                                        cart.close();
                                        }
                                        
                                        }
                                    }
                                    break;    
                                }
                            
                            
                            case '3': {
                                // Confirm purchase
                                int cartStockQuantity;
                                char cartname[100];
                                char confirmOpt;
                                cout << "Are you sure? (Y/N): ";
                                cin >> confirmOpt;
                                cin.ignore();

                                if (confirmOpt == 'Y' || confirmOpt == 'y') {
                                    fstream orderHistory("orderhis.txt", ios::out | ios::app);
                                    fstream cart("Cart.txt", ios::in);
                                    fstream tempCart("TempCart.txt", ios::out);
                                    fstream catalog("Catalog.txt", ios::in);
                                    fstream tempCatalog("Temp.txt", ios::out);

                                    bool insufficientStock = false; // Flag for stock issues

                                    // Process the cart
                                    while (cart >> fileUsername >> cartname >> Price >> cartStockQuantity >> Category >> ProductId) {
                                        if (arrayComparison(fileUsername, enteredUsername)) {
                                            // Write purchase to order history
                                            orderHistory << enteredUsername << " " << cartname << " " << Price << " " << cartStockQuantity << " " << Category << " " << ProductId << endl;

                                            // Update catalog
                                            fstream catalogTemp("Catalog.txt", ios::in);
                                            fstream tempCatalogTemp("Temp.txt", ios::out);

                                            bool itemFound = false;

                                            while (catalogTemp >> name >> Price >> StockQuantity >> Category >> ProductId) {
                                                if (arrayComparison(name, cartname)) {
                                                    itemFound = true;
                                                    if (StockQuantity >= cartStockQuantity) {
                                                        // Deduct stock if sufficient
                                                        tempCatalogTemp << name << " " << Price << " " 
                                                            << (StockQuantity - cartStockQuantity) << " " 
                                                            << Category << " " << ProductId << endl;
                                                    } else {
                                                        // Insufficient stock
                                                        cout << "Error: Insufficient stock for product " << name << ".\n";
                                                        insufficientStock = true;
                                                        tempCatalogTemp << name << " " << Price << " " 
                                                            << StockQuantity << " " << Category << " " 
                                                            << ProductId << endl;
                                                    }
                                                } else {
                                                    // Copy other items as is
                                                    tempCatalogTemp << name << " " << Price << " " 
                                                        << StockQuantity << " " << Category << " " 
                                                        << ProductId << endl;
                                                }
                                            }

                                            if (!itemFound) {
                                                cout << "Warning: Product " << cartname << " not found in catalog.\n";
                                            }

                                            catalogTemp.close();
                                            tempCatalogTemp.close();

                                            // Replace catalog with updated catalog
                                            remove("Catalog.txt");
                                            rename("Temp.txt", "Catalog.txt");

                                        } else {
                                            // Keep other users' items in the cart
                                            tempCart << fileUsername << " " << cartname << " " << Price << " " << cartStockQuantity << " " << Category << " " << ProductId << endl;
                                        }
                                    }

                                    cart.close();
                                    orderHistory.close();
                                    tempCart.close();

                                    if (!insufficientStock) {
                                        // Replace original cart with updated cart
                                        remove("Cart.txt");
                                        rename("TempCart.txt", "Cart.txt");
                                        cout << "Purchase confirmed. Your cart has been emptied.\n";
                                        finaltotal=0;
                                    } else {
                                        // Clean up temporary files in case of failure
                                        remove("TempCart.txt");
                                        cout << "Purchase failed due to insufficient stock.\n";
                                    }
                                } else {
                                    cout << "Purchase canceled.\n";
                                }
                                
                                break;
                            }


                            case '4':
                            cout<<"Going back\n";
                            break;

                            default:
                            cout<<"Invalid Input \n";

                            
                    }
                }while(opt!='4');
                break;
            }

            case '4':{

                char opt;
                do{
                cout<<"\n-------------Wishlist----------\n";
                cout<<"1) Add to wishlist\n";
                cout<<"2) Remove from Wishlist\n";
                cout<<"3) Check Wishlist\n";
                cout<<"4) Go back\n";
                cout<<"Enter your Choice: ";
                cin>>opt;
                switch(opt){
                    case '1':{
                        //add item
                        fstream catalog("Catalog.txt",ios::in);
                        fstream o1("wishlist.txt",ios::out|ios::app);
                        char enteredname [100];
                        cout<<"\nEnter Name of Item\n";
                        cin>>enteredname;
                        cin.ignore();
                        if(ifincatalog(enteredname)){
                            if(!inwishlist(enteredname)){
                            while (catalog >> name >> Price >> StockQuantity >> Category >> ProductId)
                            {
                                if(arrayComparison(enteredname,name)){
                                    o1 << enteredUsername << " " << name << " " << Price << " " << StockQuantity << " " << Category << " " << ProductId << endl;
                                    cout<<"Product Added To Wishlist Successfully.\n";
                                }
                            }
                            } else {
                                cout<<"Product already in wishlist."<<endl;
                            }
                        } else  {
                            cout<<"Product Not in Catalog.\n";
                        }
                        o1.close();
                        catalog.close();
                    
                        break;
                        }

                    case '2':{
                        //remove item
                        fstream o2("wishlist.txt",ios::in);
                        
                        char enteredname [100];
                        cout<<"\nEnter Name of Item\n";
                        cin>>enteredname;
                        cin.ignore();
                        if(inwishlist(enteredname)){
                            fstream temp("Temp.txt", ios::out);
                            while(o2>>fileUsername>>name>>Price>>StockQuantity>>Category>>ProductId){
                                // Only skip (remove) the item if it belongs to THIS user, otherwise keep it
                                if(arrayComparison(enteredname,name) && arrayComparison(fileUsername,enteredUsername)){
                                    continue;
                                } else {
                                    temp<< fileUsername << " " << name << " " << Price << " " << StockQuantity << " " << Category << " " << ProductId << endl;
                                }
                            }
                            temp.close();
                            o2.close();
                            // Only replace the real file once we know Temp.txt was actually written,
                            // otherwise we would delete the wishlist and leave nothing in its place
                            remove("wishlist.txt");
                            rename("Temp.txt", "wishlist.txt");
                            cout<<"Item Removed Successfully.\n";
                        }else{
                            o2.close();
                            cout<<"Item Not In Wishlist.\n";
                        }
                        break;


                    }

                    case '3':{//view wishlist
                        fstream o2("wishlist.txt",ios::in);
                        cout<<"-----------Your Wishlist---------\n";
                        while(o2>>fileUsername>>name>>Price>>StockQuantity>>Category>>ProductId){
                            if(arrayComparison(fileUsername,enteredUsername)){
                                cout<<name<<"   "<<Price<<"   "<<Category<<endl;
                            }
                        }
                        o2.close();
                        break;
                }

                    case '4':
                        cout<<"Going Back...\n";
                        break;

                    default:
                        cout<<"Invalid Input."<<endl;

                }

                }while(opt!='4');
                break;
            }

            case '5':{
                char opt;
                do{
                cout<<"-------------Order and FeedBack History-----------";
                cout<<"\n1) View your Order History\n"
                <<"2) Give Feedback on Orders\n"
                <<"3) Go Back\n"
                <<"Enter Your Choice: ";
                cin>>opt;
                
                switch(opt)
                {
                case '1':{
                    fstream o1("orderhis.txt",ios::in);
                    cout<<"----------------------------Your Order History------------------------\n";
                    cout << setw(15) << "Name" << setw(10) << "Price" << setw(10)<< "Stock" << setw(15) << "Category" << setw(15) << "Product ID" << endl;
                    while(o1>>fileUsername>> name >> Price >> StockQuantity >> Category >> ProductId){
                        if(arrayComparison(enteredUsername,fileUsername)){
                            cout << setw(15) << name << setw(10) << Price << setw(10)<< StockQuantity << setw(15) << Category << setw(15) << ProductId << endl;
                        }
                    }
                    o1.close();
                    break;
                }

                case '2':
                {   char productname[50];
                    char message[100];
                    cout<<"\n--------------Feedback--------------------\n";
                    cout<<"Enter the name of Product: ";
                    cin>>productname;
                    cin.ignore();
                    cout<<"Enter Feedback: ";
                    cin.getline(message,100,'\n');
                    bool itemfound = false;
                    fstream o1("feedback.txt",ios::out|ios::app);
                    fstream o2("orderhis.txt",ios::in);
                    while(o2>>fileUsername>>name>>Price>>StockQuantity>>Category>>ProductId){
                        if(arrayComparison(fileUsername,enteredUsername)&&arrayComparison(name,productname)){
                            itemfound= true;
                            o1<<fileUsername<<"|"<<name<<"|"<<message<<"|"<<endl;
                        }
                        
                    }
                    if(!itemfound){
                            cout<<"This item was never Bought.\n";
                        }

                    break;
                }
                case '3':
                    cout<<"Going Back...\n";
                    break;
                
                default:
                    cout<<"Invalid Input.\n";
                }
                }while(opt!='3');
                break;
            }

            case '6':{
                char opt;
                do
                {
                cout<<"---------Support Request--------\n";
                cout<<"1) Send a Message\n";
                cout<<"2) Go Back\n";
                cin>>opt;
                cin.ignore();
                    switch (opt)
                    {
                    case '1':
                        {
                        char message[200];
                        cout<<"Enter Your message: ";
                        cin.getline(message,200,'\n');
                        fstream o1("supportreq.txt",ios::out|ios::app);
                        o1<<enteredUsername<<"|"<<message<<"|"<<endl;
                        break;
                        }

                    case '2':
                        cout<<"Going Back.\n";    
                        break;
                    
                    default:
                        cout<<"Invalid input.\n";
                        break;
                    }
                } while (opt!='2');
                break;
            }

        
                        
                
            case '7':
                cout << "Logging out...\n";
                break;

            default:
                cout << "Invalid option selected!\n";
                break;
                
                }
        
        
        }  while (opt != '7');
    }

// Helper Functions
bool searchid(const char a[], const char b[], const char c[]) {
    // Check if username and password match any record
    fstream id("verification.txt", ios::in);
    char name1[100], password[100], role[20];
    while (id.getline(name1, 100, ',') && id.getline(password, 100, ',') && id.getline(role, 20)) {
        // Remove invisible junk (BOM / trailing '\r' from Windows-saved files)
        // so credentials still compare correctly regardless of line endings
        cleanField(name1);
        cleanField(password);
        cleanField(role);
        if (arrayComparison(a, name1) && arrayComparison(b, password) && arrayComparison(c, role)) {
            id.close();
            return true;
        }
    }
    id.close();
    return false;
}

bool searchchr(const char a[],const char b[]) {
    // Check if product exists in the cart
    fstream cart("Cart.txt", ios::in);
    while (cart >> fileUsername >> name >> Price >> StockQuantity >> Category >> ProductId) {
        if (arrayComparison(a, name) && arrayComparison(b,fileUsername)) {
            cart.close();
            return true;
        }
    }
    cart.close();
    return false;
}

bool arrayComparison(const char arr1[], const char arr2[]) {
    // Compare two character arrays
    int i = 0;
    while (arr1[i] != '\0' && arr2[i] != '\0') {
        if (arr1[i] != arr2[i])
        return false;
        i++;
    }
    return arr1[i] == '\0' && arr2[i] == '\0';
}

void changecatalog(const char a[], int newQuantity) {
    fstream catalog("Catalog.txt", ios::in);
    fstream temp("Temp.txt", ios::out);

    bool productFound = false;

    while (catalog >> name >> Price >> StockQuantity >> Category >> ProductId) {
        if (arrayComparison(name, a)) {
            productFound = true;
            if (StockQuantity >= newQuantity) {
                temp << name << " " << Price << " " 
                     << (StockQuantity - newQuantity) << " " 
                     << Category << " " << ProductId << endl;
            } else {
                cout << "Error: Insufficient stock for product " << name << ".\n";
                temp << name << " " << Price << " " 
                     << StockQuantity << " " << Category << " " 
                     << ProductId << endl;
            }
        } else {
            temp << name << " " << Price << " " 
                 << StockQuantity << " " << Category << " " 
                 << ProductId << endl;
        }
    }

    if (!productFound) {
        cout << "Warning: Product " << a << " not found in catalog.\n";
    }

    catalog.close();
    temp.close();

    // Replace original file with updated file
    remove("Catalog.txt");
    rename("Temp.txt", "Catalog.txt");
}


bool ifincatalog(const char a[]){
                            fstream catalog("Catalog.txt", ios::in);    
                            bool itemfound = false;
                            
                            while (catalog >> name >> Price >> StockQuantity >> Category >> ProductId) {
                                if (arrayComparison(name, a)) {
                                    itemfound= true;
                                }
                            }
                            return itemfound;

}

bool inwishlist(const char a[]){
    fstream o1("wishlist.txt",ios::in);
    bool itemfound = false;
    while(o1>>fileUsername>>name>>Price>>StockQuantity>>Category>>ProductId){
        
        if(arrayComparison(name,a) && arrayComparison(fileUsername,enteredUsername)){
            itemfound=true;
        }
        }
    return itemfound;
}
bool catalogcheck(const char a[],int b){
    fstream catalog("Catalog.txt",ios::in);
    bool itemFound = false;
    while (catalog >> name >> Price >> StockQuantity >> Category >> ProductId) {
                                    if (arrayComparison(name, a)) {
                                        itemFound = true;
                                        if (StockQuantity >= b) {
                                            // Add item to the cart
                                            catalog.close();
                                            return true;

                                        } else {
                                            cout << "Requested quantity not available.\n";
                                            catalog.close();
                                            return false;
                                        }
                                    }
                                }
                                if (!itemFound) {
                                    cout << "Product not found in the catalog.\n";
                                    catalog.close();
                                    return false;
                                }
                                return itemFound;
}


void admin(void){
    int choice1, choice2;
    bool loginOk = false; // tracks whether the credentials matched this attempt
    bool found;
    char announcement[200], delname[20];

    do {        
        cout << "\nLogin\n";
        cout << "-----\n";
        cout << "Enter Username: ";
        cin.getline(enteredUsername, 100);
        cout << "Enter Password: ";
        cin.getline(password, 100);
        cout << endl;

        loginOk = searchid(enteredUsername, password, "admin");
        if (loginOk) {
            cout << "Login Successful" << endl;
            storeActivity(enteredUsername, "Login Success");
        } else {
            cout << "Invalid ID or Password" << endl;
            storeActivity(enteredUsername, "Failed Login");
        }
    } while (!loginOk);

    do{
        cout << "\nAdmin\n";
        cout << "-----\n";
        cout << "1. User and Role Management\n";
        cout << "2. Product Management\n";
        cout << "3. Announcement Management\n";
        cout << "4. Activity Log\n";
        cout << "5. Sales and Revenue Analysis\n";
        cout << "6. Audit Trail\n";
        cout << "7. Logout\n";
        cout << "Enter you choice: " ;
        cin >> choice1;

        switch (choice1)
        {
        case 1:
            do{
                found = false;
                cout << "\nUser and Role Management\n";
                cout << "---------------------------\n";
                cout << "1. Add account\n";
                cout << "2. Modify account\n";
                cout << "3. Delete Account\n";
                cout << "4. Go Back\n";
                cout << "Enter you choice: ";
                cin >> choice2;

                switch (choice2)
                {
                case 1:{
                    cin.ignore();
                    cout << "Enter Role: ";
                    cin.getline(role, 100);
                    cout << "Enter Name: ";
                    cin.getline(name, 100);
                    cout << "Enter Password: ";
                    cin.getline(password, 100);

                    fstream id("verification.txt", ios::out | ios::app);
                    id << name << "," << password << "," << role <<"\n";
                    id.close();
                    cout << "ID Registered Successfully.\n\n"; 
                    storeAudit(enteredUsername, "New Account Added");               
                    break;
                }
                case 2:
                {

                    cin.ignore();
                    char newrole[20];
                    fstream id("verification.txt", ios::in);
                    fstream temp("Temp.txt", ios::out);
                    cout << "Enter Name: ";
                    cin.getline(name, 100);
                    cout << "Enter new Role: ";
                    cin.getline(newrole, 20);
                    char name1[100], password[100], role[20];
                    while (id.getline(name1, 100, ',') && id.getline(password, 100, ',') && id.getline(role, 20)) {
                        if (arrayComparison(name, name1)) {
                        temp << name1 << "," << password << "," << newrole << endl;
                        found = true;
                     
                        }
                        else{
                            temp << name1 << "," << password << "," << role << endl;
                        }
                    }
                    id.close();
                    temp.close();
                    if(!found){
                        cout << "No Such Name Found!\n";
                    }
                    else{
                        cout << "Update Success\n";
                        storeAudit(enteredUsername, "Account updated"); 
                    }

                    remove("verification.txt");
                    rename("Temp.txt", "verification.txt");
                    break;
                }
                case 3:
                {

                    cin.ignore();
                    fstream id("verification.txt", ios::in);
                    fstream temp("Temp.txt", ios::out);
                    cout << "Enter Name to delete: ";
                    cin.getline(name, 100);
                
                    char name1[100], password[100], role[20];
                    while (id.getline(name1, 100, ',') && id.getline(password, 100, ',') && id.getline(role, 20)) {
                        if (arrayComparison(name, name1)) {
                            found  = true;
                        continue;
                        }
                        else{
                            temp << name1 << "," << password << "," << role << endl;
                        }
                    }
                    id.close();
                    temp.close();

                    if(!found){
                        cout << "No Such ID found!\n";
                    }
                    else{
                        cout << "Delete Success\n";
                        storeAudit(enteredUsername, "Account deleted"); 
                    }
                    remove("verification.txt");
                    rename("Temp.txt", "verification.txt");
                
                    break;
                }
                case 4:
                    break;
                default:
            
                    break;
                }
            }while(choice2 != 4);
            break;
        case 2:
            inventorymanage(enteredUsername);
            break;
        case 3:
        do{
            cout << "\nAnnouncement Management\n";
            cout << "-------------------------\n";
            cout << "1. View Announcements\n";
            cout << "2. Add Announcement\n";
            cout << "3. Delete Announcement\n";
            cout << "4. Go Back\n";
            cout << "Enter choice: ";
            cin >> choice2;
            cin.ignore();
            switch (choice2)
            {
            case 1:
                viewannouncement();
                break;
            case 2:
                cout << "Enter you announcement to send: ";
                cin.getline(announcement, 200);
                addAnnouncement(enteredUsername, announcement);
                break; 
            case 3: 
                cout << "Enter name of person to delete announcement: ";
                cin.getline(delname, 20);
                deleteAnnouncement(delname);
                break;           
            default:
                break;
            }
        }while(choice2 != 4);
            break;
        case 4:
            viewActivities();
            break;
        case 5:
            {
                cout << "\nSales and Revenue Analysis\n";
                cout << "--------------------------\n";
                ifstream orderFile("orderhis.txt");
                if (!orderFile) {
                    cout << "Error opening file!" << endl;
                    break;
                }

                char username[50], productName[50], category[50];
                int price, stockQuantity, productId;

                int totalSales = 0;
                int topSellingValue = 0;
                char topSellingProduct[50];

                while (orderFile >> username >> productName >> price >> stockQuantity >> category >> productId) {
                    int salesValue = price * stockQuantity;
                    totalSales += salesValue;

                    if (salesValue > topSellingValue) {
                        topSellingValue = salesValue;
                        manualStrcpy(topSellingProduct, productName); 
                    }
                }
                orderFile.close();              
                cout << "Total Sales = " << totalSales << endl;
                cout << "Top Selling Product: " << topSellingProduct << " (Revenue Generated = " << topSellingValue << ")" << endl;
            }
            break;
        case 6:
            viewAudit();   
            break;
        case 7:
            cout << "Logging out...\n";
            break;     
        default:
            break;
        }
    }while(choice1 != 7);
}

void storeActivity(char username[], const char action[]) {
    fstream activityLog;
    
    activityLog.open("ActivityLogs.txt", ios::app);
    activityLog << username << ": " << action << endl; 
    activityLog.close();
}

void viewActivities() {
    fstream activityLog;
    
    activityLog.open("ActivityLogs.txt", ios::in);

    char line[300]; 
    cout << "\nActivity Logs\n";
    cout << "-------------" << endl;
    while (activityLog.getline(line, 300)) { 
        cout << line << endl;
    }
    activityLog.close();
}


void storeAudit(const char admin[], const char action[]) {
    fstream auditTrail; 

    auditTrail.open("AuditTrail.txt", ios::app);

    auditTrail << admin << ": " << action << endl; 
    auditTrail.close();
}


void viewAudit() {
    fstream auditTrail; 
    char line[300]; 
    
    auditTrail.open("AuditTrail.txt", ios::in);

    cout << "\nAudit Trail\n";
    cout << "-----------" << endl;
    while (auditTrail.getline(line, 300)) { 
        cout << line << endl;
    }
    auditTrail.close();
}

void viewannouncement() {
    fstream o1;
    char empName[20], line[100];
    cout << "\nAnnouncements\n";
    cout << "-------------\n";
    cout<<left<< setw(20) << "Employee"  << setw(100) << "Message"<<endl << endl;
    o1.open("announcement.txt",ios::in);
    while(o1.getline(empName, 20, '|') && o1.getline(line,100,'\n')){
        cout<<left<< setw(20) << empName  << setw(60) << line <<endl;
    }
    cout<<endl;
}

void addAnnouncement(const char name[], const char message[]) {
    fstream announcementFile; 

    announcementFile.open("announcement.txt", ios::app);
    announcementFile << name << "|" << message << endl; 
    announcementFile.close();
    cout << "Announcement added successfully" << endl;
}

void deleteAnnouncement(const char name[]) {
    fstream announcementFile;
    fstream tempFile;
    char line[200], empName[20];
    bool found = false;

    announcementFile.open("announcement.txt", ios::in);
    tempFile.open("Temp.txt", ios::out);

    while (announcementFile.getline(empName, 20, '|') && announcementFile.getline(line, 200, '\n')) {
        if (arrayComparison(empName, name)) {
            found = true;          
        }
        else{
            tempFile << empName << "|" << line << endl; 
        }        
    }

    announcementFile.close();
    tempFile.close();
    remove("announcement.txt");
    rename("Temp.txt", "announcement.txt");
    if (found) {
        cout << "Announcement deleted successfully" << endl;
    } else {
        cout << "No such name found" << endl;
    }
}

void alerts(){
    fstream o1;
    char name[20], Category[20];
    int price, quantity, id;

    cout << "\nAlerts\n";
    cout << "------\n";

    o1.open("Catalog.txt", ios::in);
    if(o1){
        while(o1 >> name >> price >> quantity >> Category >> id ){
            if(quantity < 15){
                cout << name << " Quantity Low " << quantity << " In Stock!" << endl;
            }
        }    
    }
    o1.close();
}

void inventorymanage(char username[]){
    int choice;
    char newname[20], newcategory[20];
    int newprice, newquantity, newid;

    do{
        cout << "\nInventory Management\n";
        cout << "---------------------\n";
        cout << "1. Add Product\n";
        cout << "2. Remove Product\n";
        cout << "3. Edit Product Quantity\n";
        cout << "4. Go Back\n";
        cout << "Enter Choice: " ;
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "Enter new Product Name: ";
            cin.ignore(); 
            cin.getline(newname, 20);            
            cout << "Enter new Product Category: ";
            cin.getline(newcategory, 20);
            cout << "Enter new Product Price: ";
            cin >> newprice;
            cout << "Enter new Product Quantity: ";
            cin >> newquantity;
            cout << "Enter new Product ID: ";
            cin >> newid;
            addProduct(newname, newprice, newquantity, newcategory, newid);  
            storeActivity(username, "New Product Added");
            break;
        case 2:
            cout << "Enter Product Name to remove: ";
            cin.ignore(); 
            cin.getline(newname, 20);
            deleteProduct(newname);
            storeActivity(username, "Product Removed");
            break;
        case 3:
            cout << "Enter Product Name to update quantity: ";
            cin.ignore(); 
            cin.getline(newname, 20);
            cout << "Enter new Quantity: ";
            cin >> newquantity;
            updateQuantity(newname, newquantity);
            storeActivity(username, "Product Quantity Updated");
            break;        
        default:
            break;
        }
    }while (choice != 4);
}

void addProduct(const char name[], int price, int stockQuantity, const char category[], int productId) {
    fstream catalogFile;
    catalogFile.open("Catalog.txt", ios::app); 

    catalogFile << name << " " << price << " " << stockQuantity << " " << category << " " << productId << endl;
    catalogFile.close();
    cout << "Product added successfully." << endl;
}

void deleteProduct(char name[]) {
    fstream catalogFile;
    fstream tempFile;
    char currentName[20], category[20];
    int price, stockQuantity, productId;
    bool found = false;

    catalogFile.open("Catalog.txt", ios::in);
    tempFile.open("Temp.txt", ios::out);

    while (catalogFile >> currentName >> price >> stockQuantity >> category >> productId) {
        if (arrayComparison(currentName, name) ) {
            found = true; 
            continue;
        }
        tempFile << currentName << " " << price << " " << stockQuantity << " " << category << " " << productId << endl;
    }
    catalogFile.close();
    tempFile.close();
    remove("Catalog.txt");
    rename("Temp.txt", "Catalog.txt");

    if (found) {
        cout << "Deleted successfully." << endl;
    } else {
        cout << "No such name found" << endl;
    }
}

void updateQuantity(char name[], int newQuantity) {
    fstream catalogFile;
    fstream tempFile;

    catalogFile.open("Catalog.txt", ios::in);
    tempFile.open("Temp.txt", ios::out);
    char currentName[20], category[20];
    int price, stockQuantity, productId;
    bool found = false;

    while (catalogFile >> currentName >> price >> stockQuantity >> category >> productId) {
        if (arrayComparison(currentName, name)) {
            found = true; 
            tempFile << currentName << " " << price << " " << newQuantity << " " << category << " " << productId << endl;
        } else {
            tempFile << currentName << " " << price << " " << stockQuantity << " " << category << " " << productId << endl;
        }
    }
    catalogFile.close();
    tempFile.close();
    remove("Catalog.txt");
    rename("Temp.txt", "Catalog.txt");

    if (found) {
        cout << "Updated successfully." << endl;
    } else {
        cout << "No such product found" << endl;
    }
}

void employee(void)
{
    char opt;
    bool loginOk = false; // tracks whether the credentials matched this attempt

    // Login functionality
    do
    {
        cout << "\nLogin\n";
        cout << "-----\n";
        cout << "Enter Username: ";
        cin.getline(enteredUsername, 100);
        cout << "Enter Password: ";
        cin.getline(password, 100);
        cout << endl;

        loginOk = searchid(enteredUsername, password, "employee");
        if (loginOk)
        {
            cout << "Login Successful" << endl;
            storeActivity(enteredUsername, "Login Success");
        }
        else
        {
            cout << "Invalid ID or Password" << endl;
            storeActivity(enteredUsername, "Failed Login");
        }
    } while (!loginOk);

    alerts();
    cout << endl;

    do
    {
        cout << "\n------- Employee Menu ---------\n";
        cout << "1. Support Request Management\n";
        cout << "2. Inventory Management\n";
        cout << "3. Feedback Management\n";
        cout << "4. Announcement Management\n";
        cout << "5. Logout\n";
        cout << "Enter choice: ";
        cin >> opt;
        switch (opt)
        {
        case '1':
        {
            char message[100];
            char responce[100];
            char opt;
            do
            {
                cout << "\n--- Support Request Management ---\n";
                cout << "1) View All Support Requests\n";
                cout << "2) Update a Support Request\n";
                cout << "3) Go Back\n";
                cout << "Enter your choice: ";
                cin >> opt;
                cin.ignore();
                switch (opt)
                {
                case '1':
                {
                    cout << "\n--------View support requests-----\n";
                    cout << left << setw(20) << "Customer" << setw(60) << "Message" << setw(60) << "Response" << endl;
                    fstream support("supportreq.txt", ios::in);
                    while (support.getline(fileUsername, 100, '|') && support.getline(message, 100, '|') && support.getline(responce, 100, '\n'))
                    {
                        cout << left << setw(20) << fileUsername << setw(60) << message << setw(20) << responce << endl;
                    }
                    support.close();
                    break;
                }
                case '2':
                {
                    char enteredmessage[100];
                    cout << "\n----Update support request----\n";
                    cout << "Enter the name of user :";
                    cin >> enteredUsername;
                    cin.ignore();
                    cout << "Enter the message";
                    cin.getline(enteredmessage, 100, '\n');
                    fstream support("supportreq.txt", ios::in);
                    fstream tempsupport("Temp.txt", ios::out);
                    char oldresponce[100];
                    // Each record is  user|message|response  — read all three fields so the
                    // response of one line doesn't bleed into the username of the next
                    while (support.getline(fileUsername, 100, '|') && support.getline(message, 100, '|') && support.getline(oldresponce, 100, '\n'))
                    {
                        if (arrayComparison(enteredUsername, fileUsername))
                        {
                            tempsupport << fileUsername << "|" << message << "|" << enteredmessage << endl;
                        }
                        else
                        {
                            tempsupport << fileUsername << "|" << message << "|" << oldresponce << endl;
                        }
                    }
                    support.close();
                    tempsupport.close();
                    remove("supportreq.txt");
                    rename("Temp.txt", "supportreq.txt");
                    break;
                }
                case '3':
                    cout << "Going Back...\n";
                    break;

                default:
                    cout << "Invalid Entry.\n";
                }
            } while (opt != '3');
            break;
        }

        case '2':
            inventorymanage(enteredUsername);
            break;
        case '3':
        {
            char message[100];
            char responce[100];
            char opt;
            do
            {
                cout << "\n--- Feedback Management ---\n";
                cout << "1) View All Feedback Requests\n";
                cout << "2) Update a Feedback Request\n";
                cout << "3) Go Back\n";
                cout << "Enter your choice: ";
                cin >> opt;

                switch (opt)
                {
                case '1':
                {
                    cout << "\n--------View Feedback requests-----\n";
                    cout << left << setw(20) << "Customer" << setw(20) << "Product" << setw(50) << "Message" << setw(50) << "Response" << endl;
                    fstream support("feedback.txt", ios::in);
                    while (support.getline(fileUsername, 100, '|') && support.getline(name, 100, '|') && support.getline(message, 100, '|') && support.getline(responce, 100, '\n'))
                    {
                        cout << left << setw(20) << fileUsername << setw(20) << name << setw(50) << message << setw(50) << responce << endl;
                    }
                    support.close();
                    break;
                }
                case '2':
                {
                    char productname[100];
                    char enteredmessage[100];
                    cout << "\n----Update Feedback request----\n";
                    cout << "Enter the name of user :";
                    cin >> enteredUsername;
                    cout << "Enter Product name: ";
                    cin >> productname;
                    cin.ignore();
                    cout << "Enter the message";
                    cin.getline(enteredmessage, 100, '\n');
                    fstream support("feedback.txt", ios::in);
                    fstream tempsupport("Temp.txt", ios::out);
                    char oldresponce[100];
                    // Each record is  user|product|message|response  — read all four fields
                    while (support.getline(fileUsername, 100, '|') && support.getline(name, 100, '|') && support.getline(message, 100, '|') && support.getline(oldresponce, 100, '\n'))
                    {
                        if (arrayComparison(enteredUsername, fileUsername) && arrayComparison(productname, name))
                        {
                            tempsupport << fileUsername << "|" << name << "|" << message << "|" << enteredmessage << endl;
                        }
                        else
                        {
                            tempsupport << fileUsername << "|" << name << "|" << message << "|" << oldresponce << endl;
                        }
                    }
                    support.close();
                    tempsupport.close();
                    remove("feedback.txt");
                    rename("Temp.txt", "feedback.txt");
                    break;
                }
                case '3':
                    cout << "Going Back...\n";
                    break;

                default:
                    cout << "Invalid Entry.\n";
                }
            } while (opt != '3');
            break;
        }
        case '4':
        {
            int choice2;
            char announcement[200], delname[20];
            do
            {
                cout << "\nAnnouncement Management\n";
                cout << "-------------------------\n";
                cout << "1. View Announcements\n";
                cout << "2. Add Announcement\n";
                cout << "3. Delete Announcement\n";
                cout << "4. Go Back\n";
                cout << "Enter choice: ";
                cin >> choice2;
                cin.ignore();
                switch (choice2)
                {
                case 1:
                    viewannouncement();
                    break;
                case 2:
                    cout << "Enter you announcement to send: ";
                    cin.getline(announcement, 200);
                    addAnnouncement(enteredUsername, announcement);
                    break;
                case 3:
                    cout << "Enter name of person to delete announcement: ";
                    cin.getline(delname, 20);
                    deleteAnnouncement(delname);
                    break;
                default:
                    break;
                }
            } while (choice2 != 4);
            break;
        }
        }

    } while (opt != '5');
}