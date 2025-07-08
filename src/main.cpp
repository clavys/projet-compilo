#include <iostream>
// #include "meta/meta_ast.h"
 
#include "meta/meta_ast.h"
using namespace std;


int main() {
    auto n1 = std::make_shared<Node>(Atom{"E", 0, AtomType::Non_Terminal});
    auto n2 = std::make_shared<Node>(Atom{"A", 0, AtomType::Non_Terminal});
    auto concat = std::make_shared<Node>(NodeKind::CONCATENATION, n1, n2);

    string test = arbreToString(S.get());
    cout << "Arbre: " << test << endl;
    return 0;
    

}