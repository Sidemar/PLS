#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

using std::vector;
using std::sort;
using std::cout;
using std::cin;
using std::endl;
using std::find;
using std::ostream;
using std::default_random_engine;
using std::uniform_int_distribution;

#define FORR(i, a) for(int i = 0; i < a; ++i)
#define pb push_back
#define sz(A) int((A).size())
#define all(v) v.begin(), v.end()
#define present(c,x) ((c).find(x) != (c).end())
#define contains(c,x) (find(all(c),x) != (c).end())
#define minLL std::numeric_limits<ll>::min()
#define V1(a) grafo[a].v1
#define V2(a) grafo[a].v2

typedef long long ll;
typedef vector<int> vi;

#define SLEEPTIME 150

int done = 0;
 
void* run (void* arg) {
    sleep(SLEEPTIME);
    kill (getpid(), SIGTERM);
    return (void *) 0;
}
 
void term(int signum) {
    done = 1;
}

class UnionFind {
private:
    UnionFind* parent;
    int rank;
    int _value;

    UnionFind(int _value) {
        this->_value = _value;
        this->parent = this;
        this->rank = 0;
    }

public:

    UnionFind() {
        this->rank = 0;
    }

    //Faz o make set de valor
    static UnionFind* makeSet(int _value) {
        UnionFind* retorno = new UnionFind(_value);
        return retorno;
    }

    int value() {
        return this->_value;
    }

    //Procura o representante e compara 
    UnionFind* Find() {
        UnionFind* current = this;
        UnionFind* last = this;
        while (current->parent != current) {
            last->parent = current->parent;
            last = current;
            current = current->parent;
        }
        return current;
    }

    //Faz uniao de dois conjuntos disjuntos
    UnionFind* Union(UnionFind* other) {
        UnionFind* root1 = other->Find();
        UnionFind* root2 = this->Find();
        if (root1->rank < root2->rank) {
            root1->parent = root2;
            return root2;
        } else if (root1->rank > root2->rank) {
            root2->parent = root1;
            return root1;
        } else {
            root2->parent = root1;
            root1->rank++;
            return root1;
        }
    }

    //Verifica se dois conjuntos tem o mesmo representante
    static bool areUnited(UnionFind* union1, UnionFind* union2) {
        return union1->Find() == union2->Find();
    }
};

class Aresta {
public:
    int id, v1, v2;
    ll peso1, peso2;

    Aresta() {
        id = 0; v1 = 0; v2 = 0; peso1 = 0LL; peso2 = 0LL;
    }

    Aresta(int v1, int v2, ll peso1, ll peso2, int id) : v1(v1), v2(v2), peso1(peso1), peso2(peso2), id(id) { }

    bool operator<(const Aresta &e) const {
        return this->peso1 < e.peso1;
    }
    
    bool operator==(const Aresta &e) const {
        return (id == e.id && v1 == e.v1 && v2 == e.v2 && peso1 == e.peso1 && peso2 == e.peso2);
    }
};

typedef vector<Aresta> va;

class Solucao {
public:
    ll val1, val2;
    vi arvore;
    bool visitado;

    Solucao() {
        val1 = 0LL;
        val2 = 0LL;
        visitado = false;
    }

    Solucao(vi &arestas, va &grafo) {
        val1 = 0LL;
        val2 = 0LL;
        visitado = false;
        ll somaObjA1 = 0LL;
        ll somaObjA2 = 0LL;
        
        FORR(i, sz(arestas)) {
            arvore.pb(arestas[i]);
        }

        FORR(i, sz(arestas)) {
            somaObjA1 += grafo[arestas[i]].peso1;
        }

        FORR(i, sz(arestas)) {
            somaObjA2 += grafo[arestas[i]].peso2;
        }

        val1 = somaObjA1;
        val2 = somaObjA2;
    }
    
    void add(Aresta a) {
        val1 += a.peso1;
        val2 += a.peso2;

        arvore.pb(a.id);
    }

    void add(int a, va grafo) {
        val1 += grafo[a].peso1;
        val2 += grafo[a].peso2;

        arvore.pb(a);
    }

    void remove(int indice, va grafo) {
        int a = arvore[indice];
        val1 -= grafo[a].peso1;
        val2 -= grafo[a].peso2;

        
        int pos = -1;

        FORR(i, sz(arvore)) {
            if (arvore[i] == a) {
                pos = i;
                break;
            }
        }
        arvore.erase(arvore.begin() + pos);
    }
};

typedef vector<Solucao> SetSol;

void lerInstancia(va &grafo) {
    int a, b;
    ll peso1, peso2;

    int id = 0;
    while(cin >> a) {
        cin >> b;
        cin >> peso1;
        cin >> peso2;
        
        grafo.pb(Aresta(a, b, peso1, peso2, id));
        id++;
    }

}

struct {
    bool operator()(Solucao *a, Solucao *b) {
        return a->val1 < b->val1;
    }
} CompSolObj1;

int rand(int a, int b) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(a, b);
    int dice_roll = distribution(generator);
    return dice_roll;
}

Solucao gerarSolucaoAleatoria(int numeroVertices, va &grafo) {
    Solucao solucaoInicial;
    UnionFind* conjDisj[numeroVertices];
    
    va grafo_aux;
    
    FORR(i, sz(grafo)) {
        grafo_aux.pb(grafo[i]);
    }
    
    FORR(i, numeroVertices) {
        conjDisj[i] = UnionFind::makeSet(i);
    }
    
    while(sz(solucaoInicial.arvore) < (numeroVertices-1)) {
        int num = rand(0, sz(grafo_aux)-1);
        Aresta a = grafo_aux[num];
        
        if(!contains(solucaoInicial.arvore, a.id)) {
            if(!UnionFind::areUnited(conjDisj[grafo_aux[num].v1], conjDisj[grafo_aux[num].v2])) {
                conjDisj[grafo_aux[num].v1]->Union(conjDisj[grafo_aux[num].v2]);
                solucaoInicial.add(a);
                grafo_aux.erase(grafo_aux.begin() + num);
            }
        }
    }
    
    FORR(i, numeroVertices) {
        delete conjDisj[i];
    }

    return solucaoInicial;
}

bool dominancia(Solucao a, Solucao b) {
    if (((a.val1 < b.val1) && (a.val2 <= b.val2))
            || ((a.val1 <= b.val1) && (a.val2 < b.val2))) {
        return true;
    } else {
        return false;
    }
}

bool dominanciaFraca(Solucao a, Solucao b) {
    return (a.val1 <= b.val1) && (a.val2 <= b.val2);
}

bool naoDominancia(Solucao a, Solucao b) {
    return (!dominanciaFraca(a, b)) && (!dominanciaFraca(b, a));
}

SetSol naoDominadas(int n, int num_vertices, va &grafo) {
    SetSol solucoes;
    
    while (solucoes.size() < n) {
        bool achouIgual = false;
        Solucao solucao = gerarSolucaoAleatoria(num_vertices, grafo);
        FORR(i, sz(solucoes)) {
            if(solucoes[i].val1 == solucao.val1 && solucoes[i].val2 == solucao.val2)
                achouIgual = true;
        }
        
        if (achouIgual) {
            continue;
        }
        
        bool domina = false;
        
        FORR(i, sz(solucoes)) {
            if(!naoDominancia(solucoes[i], solucao)) {
                domina = true;
            }
        }
        
        if(!domina) {
            solucoes.pb(solucao);
        }
    }
    
    return solucoes;
}

ll ohcv(Solucao s1, Solucao s2) {
    return (s1.val1 - s2.val1) * (s2.val2 - s1.val2);
}

ll OHI(int indice, vector<Solucao*> A) {
    if (indice == 0) {
        return 2 * ohcv(*A[indice + 1], *A[indice]);
    } else if (indice == (sz(A) - 1)) {
        return 2 * ohcv(*A[indice], *A[indice - 1]);
    } else {
        return ohcv(*A[indice + 1], *A[indice]) + ohcv(*A[indice], *A[indice - 1]);
    }
}

SetSol vizinhacaFirst(Solucao *s, int numeroVertices, va &grafo) {
    SetSol vizinhos;
    va candidatas; //arestas que estão no grafo e não estão na árvore.
    
    FORR(i, sz(grafo)) {
        bool bota = true;
        FORR(j, sz(s->arvore)) {
            if(grafo[i].id == s->arvore[j]) {
                bota = false;
                break;
            }
        }
        if(bota) candidatas.pb(grafo[i]);
    }
    
    int indiceRemovida = rand(0, sz(s->arvore)-1);
    int a1 = s->arvore[indiceRemovida];

    Solucao s_temp;
    FORR(i, sz(candidatas)) {
        Solucao p(s->arvore, grafo);
        p.remove(indiceRemovida, grafo);
        
        UnionFind* conjDisj[numeroVertices];
    
        FORR(i, numeroVertices) {
            conjDisj[i] = UnionFind::makeSet(i);
        }
        
        FORR(i, sz(p.arvore)) {
            conjDisj[V1(p.arvore[i])]->Union(conjDisj[V2(p.arvore[i])]);
        }
        
        if(conjDisj[candidatas[i].v1]->Find() != conjDisj[candidatas[i].v2]->Find()) {
            Solucao s0(p.arvore, grafo);
            s0.add(candidatas[i]);
            p.add(a1, grafo);
            
            if (dominancia(s0, p)) {
                vizinhos.pb(s0);
                FORR(i, numeroVertices) delete conjDisj[i];
                break;
            } else if (naoDominancia(p, s0) && s_temp.val1 == 0LL) {
                s_temp = s0;
            }
        }
        
        FORR(i, numeroVertices) delete conjDisj[i];
    }
    
    if (vizinhos.empty() && s_temp.val1 != 0LL) {
        vizinhos.pb(s_temp);
    }
 
    return vizinhos;
}

/*SetSol vizinhacaBest(Solucao *s, int numeroVertices, va &grafo) {
    SetSol vizinhos;
    va candidatas; //arestas que estão no grafo e não estão na árvore.
    
    FORR(i, sz(grafo)) {
        bool bota = true;
        FORR(j, sz(s->arvore)) {
            if(grafo[i].id == s->arvore[j]) {
                bota = false;
                break;
            }
        }
        if(bota) candidatas.pb(grafo[i]);
    }
    
    int indiceRemovida = rand(0, sz(s->arvore)-1);
    int a1 = s->arvore[indiceRemovida];
    
    SetSol s_temp;
    FORR(i, sz(candidatas)) {
        Solucao p(s->arvore, grafo);
        p.remove(indiceRemovida, grafo);
        
        UnionFind* conjDisj[numeroVertices];
    
        FORR(i, numeroVertices) {
            conjDisj[i] = UnionFind::makeSet(i);
        }
        
        FORR(i, sz(p.arvore)) {
            conjDisj[grafo[p.arvore[i]].v1]->Union(conjDisj[grafo[p.arvore[i]].v2]);
        }
        
        if(conjDisj[candidatas[i].v1]->Find() != conjDisj[candidatas[i].v2]->Find()) {
            Solucao s0(p.arvore, grafo);
            s0.add(candidatas[i]);
            p.add(a1, grafo);
            
            if (dominancia(s0, p)) {
                vizinhos.pb(s0);
            } else if (naoDominancia(p, s0)) {
                s_temp.pb(s0);
            }
        }
        
        FORR(i, numeroVertices) delete conjDisj[i];
    }
    
    if (vizinhos.empty() && !s_temp.empty()) {
        vizinhos = s_temp;
    }
 
    return vizinhos;
}*/

SetSol vizinhacaBest(Solucao *s, int numeroVertices, va &grafo) {
   SetSol vizinhos;
   va candidatas; //arestas que estão no grafo e não estão na árvore.
   
   FORR(i, sz(grafo)) {
       bool bota = true;
       FORR(j, sz(s->arvore)) {
           if(grafo[i].id == s->arvore[j]) {
               bota = false;
               break;
           }
       }
       if(bota) candidatas.pb(grafo[i]);
   }
   
   int indiceRemovida = rand(0, sz(s->arvore)-1);
   int a1 = s->arvore[indiceRemovida];
   
   SetSol s_temp;
   FORR(i, sz(candidatas)) {
       Solucao p(s->arvore, grafo);
       p.remove(indiceRemovida, grafo);
       
       UnionFind* conjDisj[numeroVertices];
   
       FORR(i, numeroVertices) {
           conjDisj[i] = UnionFind::makeSet(i);
       }
       
       FORR(i, sz(p.arvore)) {
           conjDisj[grafo[p.arvore[i]].v1]->Union(conjDisj[grafo[p.arvore[i]].v2]);
       }
       
       if(conjDisj[candidatas[i].v1]->Find() != conjDisj[candidatas[i].v2]->Find()) {
           Solucao s0(p.arvore, grafo);
           s0.add(candidatas[i]);
           p.add(a1, grafo);
           
           if (naoDominancia(p, s0)) {
               vizinhos.pb(s0);
           }
       }
       
       FORR(i, numeroVertices) delete conjDisj[i];
   }
   
   if (vizinhos.empty() && !s_temp.empty()) {
       vizinhos = s_temp;
   }

   return vizinhos;
}

/*void filter(SetSol &A) {
    SetSol removidas;
    FORR(i, sz(A)) {
        FORR(j, sz(A)) {
            if (dominancia(A[i], A[j])) {
                removidas.pb(A[j]);
            }
        }
    }
    
    FORR(i, sz(removidas)) {
        FORR(j, sz(A)) {
            if (removidas[i].val1 == A[j].val1 && removidas[i].val2 == A[j].val2) {
                A.erase(A.begin()+j);
                j--;
            }
        }
    }
}*/

double tempo() {
    timeval _start;
    gettimeofday(&_start, 0);
    return _start.tv_sec + (_start.tv_usec/1000000.0);
}

void filter(SetSol &A, Solucao so) {
    bool dominado = false;
    
    FORR(i, sz(A)) {
        dominado = dominancia(A[i], so);
        if (dominado || (so.val1 == A[i].val1 && so.val2 == A[i].val2)) return;
        if (dominancia(so, A[i])) {
            std::swap(A[i], A.back());
            A.pop_back();
            i--;
        }
    }
    A.pb(so);
}


void PLS() {
    int n, m, qnt_nao_domin;
    va grafo;
    
    struct sigaction action;
    memset(&action, 0, (size_t) sizeof (struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    pthread_t thread;

    scanf("%d", &n);
    m = (n*(n+1))/2;
    grafo.reserve(m);
    
    //Usado para medir o tempo do algoritmo
    double t1, t2, tempo_total, tempo_leitura, tempo_gerar, tempo_algortimo;
    t1 = tempo();
    lerInstancia(grafo);
    t2 = tempo();
    
    tempo_leitura = (t2-t1);
    tempo_total = tempo_leitura;
    
    pthread_create(&thread,NULL,&run,NULL);
    
    t1 = tempo();
    qnt_nao_domin = int(0.2*n);
    
    SetSol A = naoDominadas(qnt_nao_domin, n, grafo);
    t2 = tempo();
    
    tempo_gerar = (t2-t1);
    tempo_total += tempo_gerar;
    
    t1 = tempo();
    vector<Solucao*> A0;
    
    int itera = 0;
    
    FORR(i, sz(A)) {
        A0.pb(&A[i]);
    }
    
    // while ((sz(A0) > 0) && (done != 1)) {
        
    //     itera++;
        
    //     sort(A0.begin(), A0.end(), CompSolObj1);
        
    //     int indexMaior = -1;
    //     ll ohiMaior = minLL;
    //     ll ohiAtual = 0LL;
        
    //     if (sz(A0) > 1) {
    //         FORR(i, sz(A0)) {
    //             ohiAtual = OHI(i, A0);
    //             if (ohiAtual > ohiMaior) {
    //                 indexMaior = i;
    //                 ohiMaior = ohiAtual;
    //             }
    //         }
    //     } else {
    //         indexMaior = 0;
    //     }
        
    //     Solucao* s = A0[indexMaior];
    //     A0.erase(A0.begin() + indexMaior);
        
    //     SetSol vizinhos = vizinhacaFirst(s, n, grafo);
        
    //     if(sz(vizinhos) > 0) {
    //         FORR(i, sz(vizinhos)) {
    //             vizinhos[i].visitado = false;
    //             filter(A, vizinhos[i]);
    //         }
    //     }
        
    //     s->visitado = true;
        
    //     A0.clear();
    //     FORR(i, sz(A)) {
    //         if(!A[i].visitado) {
    //             A0.pb(&A[i]);
    //         }
    //     }
    // }
    
    FORR(i, sz(A)) {
        A[i].visitado = false;
        A0.pb(&A[i]);
    }
    
    while ((sz(A0) > 0) && (done != 1)) {
        
        itera++;
        
        sort(A0.begin(), A0.end(), CompSolObj1);
        
        int indexMaior = -1;
        ll ohiMaior = minLL;
        ll ohiAtual = 0LL;
        
        if (sz(A0) > 1) {
            FORR(i, sz(A0)) {
                ohiAtual = OHI(i, A0);
                if (ohiAtual > ohiMaior) {
                    indexMaior = i;
                    ohiMaior = ohiAtual;
                }
            }
        } else {
            indexMaior = 0;
        }
        
        Solucao* s = A0[indexMaior];
        A0.erase(A0.begin() + indexMaior);
        
        SetSol vizinhos = vizinhacaBest(s, n, grafo);
        
        if(sz(vizinhos) > 0) {
            FORR(i, sz(vizinhos)) {
                vizinhos[i].visitado = false;
                filter(A, vizinhos[i]);
            }
        }
        
        s->visitado = true;
        
        A0.clear();
        FORR(i, sz(A)) {
            if(!A[i].visitado) {
                A0.pb(&A[i]);
            }
        }
    }
    
    t2 = tempo();
    tempo_algortimo = (t2-t1);
    tempo_total += tempo_algortimo;
    
    cout << "Leu instância. Demorou " << tempo_leitura << " segundo(s)." << endl;
    cout << "Gerou " << qnt_nao_domin << " soluções não-dominadas iniciais. Demorou " << tempo_gerar << " segundo(s)." << endl;
    cout << "Terminou algortimo (PLS-AnyTime). Demorou " << tempo_algortimo << " segundos(s)." << endl;
    cout << "TEMPO TOTAL " << tempo_total << " segundos(s)." << endl ;
    cout << "ITERACOES " << itera << endl << endl;
    
    cout << "Pareto Front" << endl << endl;
    FORR(i, sz(A)) {
        cout << A[i].val1 << " " << A[i].val2 << endl;
    }
}

int main(int argc, char** argv) {
    PLS();
    return 0;
}