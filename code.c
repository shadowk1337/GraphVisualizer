#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define EM_EXIT exit(EXIT_FAILURE)
#define SIZE_S(s) strlen(s)
#define FALSE_TERM return(false)
#define TRUE_TERM return(true)
#define SIZE 1000

typedef struct List{
    int32_t parent_node_value;
    int32_t *linked_numbers;
    uint32_t ar_size;
    struct List *next;
} List;

List *head = NULL;
int is_simple = 1;

_Bool ParseInput(char **s); // проверка правильности пользовательского ввода (возможны варианты "   33 --   2", "1--2", "1    --   3    ")
void Add(int32_t a, int32_t b);
void Out(FILE *f);
_Bool CheckComplexity(void);
_Bool CheckConnectivity(void);

int main(void)
{
    char *to_add;
    int32_t p_n_value, linked_number;
    FILE *f;
    puts("Enter the pair of vertex numbers, Ex: \"1 -- 2\", \"1    -- 3\"(q to exit and output the asnwer)");
    while (true){
        printf("> ");
        while(!ParseInput(&to_add)){
            fflush(stdin);
            puts("Enter a correct sentence: ");
            printf("> ");
        }
        if (strcmp(to_add, "q") == 0)
            break;
        sscanf(to_add, "%d--%d", &p_n_value, &linked_number);
        Add(p_n_value, linked_number);
    }
    if ((f = fopen("graph.dot", "w")) == NULL){
        printf("Couldn't open the file %s", "graph.dot");
        EM_EXIT;
    };
    Out(f);
//    system("dot -Tpng graph.dot -o a.png");
    CheckConnectivity();
    fclose(f);
    return 0;
}

//===================================================
// Обработка ввода + добавление в список + вывод в файл
_Bool CheckAndFixInput(char *s);

_Bool ParseInput(char **s){
    char input[SIZE];
    if ((fgets(input, SIZE, stdin)) != NULL){
        input[SIZE_S(input) - 1] = '\0';
        if (strcmp(input, "q") == 0){
            strcpy(*s, "q");
            TRUE_TERM;
        }
        if (CheckAndFixInput(input)){
            *s = (char *)malloc(SIZE_S(input));
            strcpy(*s, input);
            TRUE_TERM;
        }
    }
    FALSE_TERM;
}

void RemoveSpaces(char *s){ // удаление пробелов
    const char* d = s;
    do{
        while (*d == ' ' || *d == '\n')
            ++d;
    }while ((*s++ = *d++));
}

char * CheckDigit(char *s){ // проверка на число
    char *p = s;
    while(isdigit(*p))
        p++;
    return p;
}

#define DIGITANDSPACECHECK(p, s) \
    RemoveSpaces(s); \
    p = CheckDigit(s); \
    RemoveSpaces(s)

_Bool CheckAndFixInput(char *s){
    char *p_check;
    DIGITANDSPACECHECK(p_check, s);
    if (*p_check == '-' && *(++p_check) == '-'){
       if(*++p_check == '-')
           FALSE_TERM;
       DIGITANDSPACECHECK(p_check, s);
       if (!isdigit(*p_check))
           TRUE_TERM;
    }
    FALSE_TERM;
}

_Bool FindParentsValue(int32_t a, List **it){
    while (*it != NULL){
        if ((*it)->parent_node_value == a)
            TRUE_TERM;
        if ((*it)->next == NULL)
            FALSE_TERM;
        *it = (*it)->next;
    }
    FALSE_TERM;
}

_Bool SearchInArray(const int32_t val, int32_t *ar, uint32_t size){
    for (uint32_t i = 0; i < size; i++){
        if (val == ar[i])
            TRUE_TERM;
    }
    FALSE_TERM;
}

#define INIT(x) \
    x = (List *)malloc(sizeof(List)); \
    x->linked_numbers = malloc(sizeof(int32_t)); \
    x->parent_node_value = a; \
    *(x->linked_numbers) = b; \
    x->ar_size = 1; \
    x->next = NULL

#define IT(x) iterator->x

void Add(int32_t a, int32_t b){
    List *iterator = head;
    if (head == NULL){
        INIT(head);
        return;
    }
    int q = FindParentsValue(a, &iterator);
    if (!q){
        INIT(IT(next));
    }
    else{
        if (IT(parent_node_value) == a && SearchInArray(b, IT(linked_numbers), IT(ar_size)))
            return;
        IT(linked_numbers) = realloc(IT(linked_numbers), ++IT(ar_size));
        *(IT(linked_numbers) + (IT(ar_size) - 1)) = b;
    }
}

void Out(FILE *f){
    List *iterator = head;;
    fprintf(f, "%s", "graph g {\n");
    while (iterator != NULL){
        for (uint32_t i = 0; i < IT(ar_size); i++)
            fprintf(f, "%d -- %d;\n", IT(parent_node_value), *(IT(linked_numbers) + i));
        iterator = IT(next);
    }
    fprintf(f, "%s", "}");
}

//===================================================
// Работа с графом через список
#define SEARCHNUMB(x, y) SearchInArray(x->parent_node_value, y->linked_numbers, y->ar_size)

_Bool CheckComplexity(void){
    for (List *it_x = head; it_x != NULL; it_x = it_x->next){
        for (List *it_y = head; it_y != NULL; it_y = it_y->next){
            if (SEARCHNUMB(it_x, it_y) && SEARCHNUMB(it_y, it_x))
                TRUE_TERM;
        }
    }
    FALSE_TERM;
}

int32_t VertexCount(void){
    int32_t result = 0;
    bool flag = 0;
    for (List *it_x = head; it_x != NULL; it_x = it_x->next){
        result++;
        for (uint32_t i = 0; i < it_x->ar_size; i++){
            for (List *it_y = head; it_y != NULL; it_y = it_y->next){
                if (it_x->linked_numbers[i] == it_y->parent_node_value)
                    flag = 1;
            }
        }
        if (!flag)
            result++;
        flag = 0;
    }
    return result;
}

int32_t EdgeCount(void){
    int32_t result = 0;
    for (List *it = head; it != NULL; it = it->next)
        result += it->ar_size;
    return result;
}

_Bool CheckConnectivity(void){
    int32_t vertex_count = VertexCount();
    int32_t edge_count = EdgeCount();
    int32_t formule = (vertex_count - 1)*(vertex_count - 2) / 2;
    printf("%d, %d\n", vertex_count, edge_count);
    if (CheckComplexity()){
        puts("Graph is not simple!");
        FALSE_TERM;
    }
    else if (edge_count > formule){
        puts("Graph is connected");
        TRUE_TERM;
    }
    else{
        puts("Graph is not connected");
        FALSE_TERM;
    }
}
