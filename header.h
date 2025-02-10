#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>

#include <string.h>



//
//  Definitions
//


/*****************************************************************************
Fonction:End_verificator(): Fonction qui verifi la correspondance de chaine de caract�re
Entr�e: char *ch_input: chaine de caract�re lu dans le buffer pour la comparaison
       const char *ch_cor: chaine voulu, c'est la chaine que l'on veut trouver dans la lecture courante
Sortie:la fonction retourne 1 si la correspond reussi et 0 sinon
*****************************************************************************/

int End_verificator(const char *ch_input,const char *ch_cor)
{
    //Recuperation de la longeure des chaines
    int len1 = strlen(ch_input);
    int len2 = strlen(ch_cor);

    //Si la longeur chaine lu � partir du buffer est toujours inferieur � la chaine voulu
    if (len1 < len2)
        return 0; //On ne peut pas matcher la corrspondance des chaines

    //Si la chaine lu sur le buffer est superieur ou egale � la chaine voulu
    //Debuter la comparaison avec les caract�res
    for (int i = 0; i <len1; i++)
    {
        if (ch_input[len1 - len2 + i] != ch_cor[i])
            return 0;
    }
    //Si le match entre les caract�res est correcte
    return 1;
}

/**Structure d'un attribut ******/

typedef struct _XMLAttribute
{
    char* key;// Nom de l'attribut
    char* value;// Valeur de l'attribut
}XMLAttribute;

void XMLAttribute_free(XMLAttribute* attr); //Declaration du protptype de liberation d'un attribut

/**Structure d'une Liste attributs ******/

typedef struct _XMLAttributeList
{
    int ind_size;//Indicateur pour l'allocation de la memoire
    int size; //Nombre d'element de la liste d'attribut
    XMLAttribute *data;//Tableau d'attribut
}XMLAttributeList;

//Declaration du protptype des fonctions initialisation et d'ajout pour les attributs
void XMLAttributeList_init(XMLAttributeList* list);
void XMLAttributeList_add(XMLAttributeList* list, XMLAttribute* attr);

/**Structure d'une Liste de Noeuds******/
typedef struct _XMLNodeList
{
    int ind_size;//Indicateur pour l'allocation de la memoire
    int size;//Nombre d'element de la liste de noeud
    struct _XMLNode **data;//Tableau de noeud
}XMLNodeList;

//Declaration du protptype des fonctions initialisation et d'ajout pour les noeuds
void XMLNodeList_init(XMLNodeList* list);
void XMLNodeList_add(XMLNodeList* list, struct _XMLNode *node);
struct _XMLNode* XMLNodeList_at(XMLNodeList* list, int index);//Recuperation d'un noeud en fonction de son index
void XMLNodeList_free(XMLNodeList* list);//Liberation de l'espace memoire pour un noeud

/**Structure d'un Noeud ******/
typedef struct _XMLNode
{
    char    *tag; //Nom du noeud
    char *inner_text; //Contenu textuel du noeud
    struct   _XMLNode *parent; //Noeud parent du noeud actuel
    XMLAttributeList attributes; //La liste d'attribut du noeud
    XMLNodeList children; //La liste de neoud du Noeud actuel
}XMLNode;


XMLNode* XMLNode_new(XMLNode* parent);//protptype pour la creation d'un noeud
void XMLNode_free(XMLNode* node);//protptype pour la liberation d'un noeud
XMLNode* XMLNode_child(XMLNode* parent, int index);//protptype pour la recuperation d'un fils � partir de noeud p�re
XMLNodeList* XMLNode_children(struct _XMLNode* parent, const char* tag);//protptype pour la recuperation d'un fils � partir de noeud p�re en utilisant son tag
char* XMLNode_attr_val(XMLNode* node, char* key);//protptype pour la recuperation de la valeur d'un attribut
XMLAttribute* XMLNode_attr(XMLNode* node, char* key);//protptype pour la recuperation d'un attribut � partir de son nom


/**Structure du document xml ******/
typedef struct _XMLDocument
{
    XMLNode *root;//Noeud racine du document
    char *version;//version du XML
    char *encoding;//Encodage
}XMLDocument;


int XML_load(XMLDocument* doc, const char* path);//Prototype pour la lecture du fichier xml
int XMLDocument_write(XMLDocument* doc, const char* path, int indent); //Prototype pour ecrire les donn�es lu dans un autre fichier
void XMLDocument_free(XMLDocument* doc); //Prototype pour la liberation de la memoire

//
//  Implementation
//
/*****************************************************************************
Fonction:XMLAttribute_free(): Fonction qui lib�re l'espace allou� pour un attribut
Entr�e: XMLAttribute* attr:Attribut � liber�
Sortie:void
*****************************************************************************/
void XMLAttribute_free(XMLAttribute* attr)
{
    free(attr->key);//Liberattion de  la cl�
    free(attr->value);//Liberattion de la valeur de l'attribut
}
/*****************************************************************************
Fonction:XMLAttributeList_init(): Fonction qui alloue l'espace allou� pour un attribut
Entr�e: XMLAttributeList* list:La liste d'attribut � initialis�
Sortie:void
*****************************************************************************/
void XMLAttributeList_init(XMLAttributeList* list)
{
    list->ind_size= 1;
    list->size = 0;//Nombre d'element
    list->data = (XMLAttribute*)malloc(sizeof(XMLAttribute) * list->ind_size);
    if(!list->data)
    {
        //En cas d'echec d'allocation
        printf("Erreur d'allocation \n");
        exit(-1);
    }
}
/*****************************************************************************
Fonction:XMLAttributeList_add(): Fonction qui ajoute un attribut � une liste d'attribut
Entr�e: XMLAttributeList* list: Liste d'attribut
        XMLAttribute* attr: L'attribut � ajouter
Sortie:void
*****************************************************************************/
void XMLAttributeList_add(XMLAttributeList* list, XMLAttribute* attr)
{
    //Si l'espace allou� ne peut pas contenir l'attribut � ajouter, reallouer la memoire
    while (list->size >= list->ind_size)
    {
        list->ind_size *= 2;
        list->data = (XMLAttribute*) realloc(list->data, sizeof(XMLAttribute) * list->ind_size);
    }
    //Ajout de l'attribut
    list->data[list->size++] = *attr;
}
/*****************************************************************************
Fonction:XMLNodeList_init(): Fonction qui initialise une liste pour l'ajout des noeuds
Entr�e: XMLNodeList* lis: Liste de noeud
Sortie:void
*****************************************************************************/
void XMLNodeList_init(XMLNodeList* list)
{

    list->ind_size = 1;
    list->size = 0;//Nombre d'element
    list->data = (XMLNode**) malloc(sizeof(XMLNode*) * list->ind_size);
    if(!list->data)
    {
         //En cas d'echec d'allocation
        printf("Erreur d'allocation \n");
        exit(-1);
    }
}
/*****************************************************************************
Fonction:XMLNodeList_add(): Fonction qui pour l'ajout des noeuds � une liste de noeud
Entr�e: XMLNodeList* lis: Liste de noeud
       XMLNode* node: Le noeud � ajouter
Sortie:void
*****************************************************************************/
void XMLNodeList_add(XMLNodeList* list, XMLNode* node)
{
    //Si l'espace allou� ne peut pas contenir l'attribut � ajouter, reallouer la memoire
    while (list->size >= list->ind_size)
    {
        list->ind_size *= 2;
        list->data = (XMLNode**) realloc(list->data, sizeof(XMLNode*) * list->ind_size);
    }
    //Ajout du Noeud
    list->data[list->size++] = node;
}
/*****************************************************************************
Fonction:XMLNodeList_at(): Fonction qui renvoie un noeud
Entr�e: XMLNodeList* list:  Liste de Noeud
        int index:Indice du Noeud � retourner
Sortie:La fonction retourne le noeud � l'indice pass� en param�tre
*****************************************************************************/
XMLNode* XMLNodeList_at(XMLNodeList* list, int index)
{
    return list->data[index];
}
/*****************************************************************************
Fonction:XMLNodeList_free(): Fonction qui lib�re l'espace allou� pour les noeuds
Entr�e: XMLNodeList* list:  Liste de Noeud
Sortie:void
*****************************************************************************/
void XMLNodeList_free(XMLNodeList* list)
{
    free(list);
}

/*****************************************************************************
Fonction:XMLNode_new(): Fonction qui cr�e un  nouveau noeud
Entr�e: XMLNode* parent:Noeud p�re � ajouter
Sortie:La fonction retourne le noeud cr�e
*****************************************************************************/
XMLNode *XMLNode_new(XMLNode *parent)
{
    XMLNode* node = (XMLNode*) malloc(sizeof(XMLNode));//Allocation
    if(!node)
    {
        //Si l'allocation echoue quitter
        printf("Erreur d'allocation de la memoire ");
        exit(-1);
    }
    //affection des informations
    node->parent = parent;
    node->tag = NULL;
    node->inner_text = NULL;

    XMLAttributeList_init(&node->attributes);//Initialisation des attributs du  noeuds
    XMLNodeList_init(&node->children);
    if (parent)
        XMLNodeList_add(&parent->children, node);//Si le parent n'est pas nul ajoute le nouveau noeud comme enfant
    return(XMLNode*)node;
}

/*****************************************************************************
Fonction:XMLNodeList_free(): Fonction qui lib�re l'espace allou� pour le noeud
Entr�e: XMLNode* node:  Noeud � liber�
Sortie:void
*****************************************************************************/
void XMLNode_free(XMLNode* node)
{
    if (node->tag)
        free(node->tag);//Liber� le tag
    if (node->inner_text)
        free(node->inner_text);//Liber� le contenu textuel
    for (int i = 0; i < node->attributes.size; i++)
        XMLAttribute_free(&node->attributes.data[i]);//Liber� les attributs du noeud
    free(node);//Liberation du noeud
}
/*****************************************************************************
Fonction:XMLNode_child(): Fonction qui retourne le noeud fils specifi� par son index
Entr�e: XMLNode* parent: Noeud parent
        int index: index de noeud fils
Sortie:la fonction retourne le noeud fils index�
*****************************************************************************/
XMLNode *XMLNode_child(XMLNode* parent, int index)
{
    return parent->children.data[index];
}
/*****************************************************************************
Fonction:XMLNode_children():Fonction qui retourne la liste des noeuds fils specifi� par leur tag
Entr�e: XMLNode* parent: Noeud parent
        const char tag:nom du noeud fils
Sortie:la fonction retourne la liste des fils index�
*****************************************************************************/
XMLNodeList *XMLNode_children(XMLNode* parent, const char* tag)
{
    //Allocation de la memoire
    XMLNodeList* list = (XMLNodeList*) malloc(sizeof(XMLNodeList));
        if(!list)
        {
            //Echec de l'allocation
            printf("ERREUR D'ALLOCATION \n");
            exit(-1);
        }
        //Initialiser la lister
    XMLNodeList_init(list);

    for (int i = 0; i < parent->children.size; i++)
    {
        //En parcourant les noeuds enfants les inser�s dans une liste s'ils correspondent � leur tag
        XMLNode* child = parent->children.data[i];
        if (!strcmp(child->tag, tag))
            XMLNodeList_add(list, child);
    }

    return(XMLNodeList*) list;
}
/*****************************************************************************
Fonction:xmlGetProp():Fonction de traitement de noeud
Entr�e: XMLNode* node: Noeud � traiter
        char* key: Nom de l'attribut
Sortie:la fonction retourne la valeur de l'attribut
*****************************************************************************/
char *xmlGetProp(XMLNode *node, char *key)
{
    //On parcourt la liste d'attributs jusqu'� trouver l'attribut specifi�
    for (int i = 0; i < node->attributes.size; i++)
    {
        XMLAttribute attr = node->attributes.data[i];
        if (!strcmp(attr.key, key))
            return attr.value;//Retourner la valeur
    }
    return NULL;
}
/*****************************************************************************
Fonction:XMLNode_attr():Fonction de traitement de noeud
Entr�e: XMLNode* node: Noeud � traiter
        char* key: Nom de l'attribut
Sortie:la fonction retourne l'attribut
*****************************************************************************/
XMLAttribute* XMLNode_attr(XMLNode* node, char* key)
{
    for (int i = 0; i < node->attributes.size; i++)
    {
        XMLAttribute* attr = &node->attributes.data[i];
        if (!strcmp(attr->key, key))
            return attr;
    }
    return NULL;
}

typedef enum _TagType
{
    TAG_START,// tag pour specifier ceux qui sont des balises avec des enfants
    TAG_INLINE// tag pour specifier ceux qui sont des balises orphelines
}TagType;

/*****************************************************************************
Fonction:TagType parse_attribut():Fonction de traitement des noeuds et leur attributs
Entr�e: char *buf: Buffer contenant l'ensemeble des caract�res � analyser
            int *i:Indice courante de lecture dans le buffer
            char *lex: chaine de caract�re contenant pour stoker la chaine courante
            int *lexi:indice pour le stockage de la chaine � analyser
            XMLNode* curr_node: Noeud courant �  traiter
Sortie:la fonction retourne un type de flag montrant le type de  noued lu
*****************************************************************************/
static TagType parse_attribut(char* buf, int* i, char* lex, int* lexi, XMLNode* curr_node)
{
    XMLAttribute curr_attr = {0, 0};//Initialisation de l'attribue courante
    //On lit caract�re par caract�re jusqu'� trouver un element distictif
    while (buf[*i] != '>')
    {
        lex[(*lexi)++] = buf[(*i)++];//Recuperer le 1er caract�tre



        //  cas du nom du noeud "Tag name"
        if ((buf[*i] == ' ' )&& (!curr_node->tag))
        {
            lex[*lexi] = '\0';
            curr_node->tag = strdup(lex);//donner au noeud le nom stocker dans la  variable lex
            *lexi = 0;//Renitialiser la position pour restocker de nouveau caract�re au debut de lex
            (*i)++;//Passer � la position suivante car la position courante est un  espace
            continue;
        }
        //Si le carat�re suivant est un espace
         // Usually ignore spaces
        if (lex[*lexi-1] == ' ')
        {
            (*lexi)--;
        }

        //Si le carat�re suivant est le caract�re egal
        // cas d'une "Attribute key"
        if (buf[*i] == '=')
        {
            lex[*lexi] = '\0';

            curr_attr.key = strdup(lex);//donner � l'attribut courant le nom stocker dans la  variable lex
            *lexi = 0;//Renitialiser la position pour restocker de nouveau caract�re au debut de lex
            continue;
        }

        //Si le carat�re suivant est le caract�re est double quote
        // cas Attribute value
        if (buf[*i] == '"')
        {
            //Si l'attribut courante n'a pas de cl� alors c'est une syntaxe mauvaise
            if (!curr_attr.key)
            {
                fprintf(stderr, "Value has no key\n");
                return TAG_START;
            }
            //Sinon recuperer toute les caract�res suivant jusqu'� trouver un espace
            *lexi = 0;
            (*i)++;
            while (buf[*i] != '"')
                lex[(*lexi)++] = buf[(*i)++];
            lex[*lexi] = '\0';
            curr_attr.value = strdup(lex);//Enregister la chaine suivante comme valeur d'attribut
            XMLAttributeList_add(&curr_node->attributes, &curr_attr);//Enregister l'attribut dans la liste d'attribut
            //Renitialiser l'attribut temporaire
            curr_attr.key = NULL;
            curr_attr.value = NULL;
            *lexi = 0;//renitialiser la position de l'indice de stockage
            (*i)++;//Avancer l'indice de lecture du buffer
            continue;
        }

        //Si on croise le caract�re "/" cela veux dire que c'est une balise unique elle n'a pas d'enfant
        // Inline node
        if (buf[*i - 1] == '/' && buf[*i] == '>')
        {
            lex[*lexi] = '\0';
            if (!curr_node->tag)
                curr_node->tag = strdup(lex);
            (*i)++;
            return TAG_INLINE;
        }
    }


    return TAG_START;
}


/*****************************************************************************
Fonction:XML_load():Fonction qui se charge du parsing des noeuds
Entr�e: char *buf: Buffer contenant l'ensemeble des caract�res � analyser
        XMLDocument* doc: Document Xml pour le traitement
Sortie:la fonction retourne 1 si la l'operation � �t� un succ�s
*****************************************************************************/
int XML_load(XMLDocument* doc,const char *buf)
{


   // return XML_load(doc,buf);;


    doc->root = XMLNode_new(NULL);//Creation de la racine

    char lex[1000];// Chaine de caract�re temporaire pour recuper les elements du buffer
    int lexi = 0;//indice  de stockage dans la chaine temporaire
    int i = 0;  //Indice courant pour la lecture dans le buffer

    XMLNode* curr_node = doc->root;//Recuperation du noeud racine

    //On lis le buffer jusqu'� trouver la fin de la chaine caract�re
    while (buf[i] != '\0')
    {
        //Si c'est un  noeud
        if (buf[i] == '<')
        {
            lex[lexi] = '\0';


            // Inner text
            if (lexi > 0)
            {
                //Si on a du texte � l'exterieur du noeud
                if (!curr_node)
                 {//Erreur
                    fprintf(stderr, "Text outside of document\n");
                    return 0;
                }
                //Si on on le texte  et que le noeud est cr�e
                curr_node->inner_text = strdup(lex);//Ajouter le contenu comme innertext du noeud
                lexi = 0;//Remettre l'index de la position de stockage � zero
            }

            // End of node
            if (buf[i + 1] == '/')
            {
                i += 2;
                while (buf[i] != '>')
                    lex[lexi++] = buf[i++];//Recuperer le tag de la fermeture du noeud
                lex[lexi] = '\0';

                if (!curr_node)
                {//Si le noud n'est pas creer, erreur
                    fprintf(stderr, "Manque de Manque\n");
                    return 0;
                }
                //Sinon , comparer la chaine de caract�re de la balise fermante avec le nom du noeud
                if (strcmp(curr_node->tag, lex))
                {//Si les chaines sont differentes alors erreur
                    fprintf(stderr, "Mismatched tags (%s != %s)\n", curr_node->tag, lex);
                    return 0;
                }
                //Sinon,Recuperer le p�re du noeud
                curr_node = curr_node->parent;
                i++;
                continue;
            }

            // Noeud particulier
                if (buf[i + 1] == '!')
                {
                    while (buf[i] != ' ' && buf[i] != '>')
                        lex[lexi++] = buf[i++];
                    lex[lexi] = '\0';

                    // Cas d'un noeud commentaire
                    if (!strcmp(lex, "<!--"))
                        {
                            lex[lexi] = '\0';
                            //Chercher la fin du noeud commentaire
                            while (!End_verificator(lex, "-->"))
                            {
                                lex[lexi++] = buf[i++];
                                lex[lexi] = '\0';
                            }
                            continue;
                        }
                }

                // Declaration tags
                //Noeud de description du document xml
                if (buf[i + 1] == '?')
                {
                        while (buf[i] != ' ' && buf[i] != '>')
                            lex[lexi++] = buf[i++];
                        lex[lexi] = '\0';

                        //Si la chaine correspond � la declaration xml
                        if (!strcmp(lex, "<?xml"))
                        {
                            lexi = 0;
                            XMLNode* desc = XMLNode_new(NULL);//Creer un noeud pour enregistrer le document xml
                            parse_attribut(buf, &i, lex, &lexi, desc);//Parser tout ses attribut

                            doc->version =xmlGetProp(desc, "version");//Recuperer la version
                            doc->encoding = xmlGetProp(desc, "encoding");//Recuperer l'encodage

                            continue;
                        }
                }

                    // Creer le noeud courant
                    curr_node = XMLNode_new(curr_node);

                    // Si on a un noeud sans fils
                    i++;
                    if (parse_attribut(buf, &i, lex, &lexi, curr_node) == TAG_INLINE)
                    {
                        curr_node = curr_node->parent;
                        i++;
                        continue;
                    }

                // Set tag name if none
                lex[lexi] = '\0';
                if (!curr_node->tag)
                    curr_node->tag = strdup(lex);

                // Reset lexer
                lexi = 0;
                i++;
                continue;

         }
         else
        {
            lex[lexi++] = buf[i++];
        }
    }

    return 1;
}
/*****************************************************************************
Fonction:XMLDOC_Loading():Fonction recup�re les donner d'un fichier xml
Entr�e: XMLDocument* doc: Document Xml pour le traitement
        const char* path: Chemin du fichier
Sortie:la fonction retourne 1 si la l'operation � �t� un succ�s
*****************************************************************************/
int XMLDOC_Loading(XMLDocument* doc,const char* path)
{
    FILE* file = fopen(path, "r");//Ouverture du fichier xml
    if (!file)
    {//Si l'ouverture � �t� un echec , quitter
        fprintf(stderr, "Could not load file from '%s'\n", path);
        exit(-1);
    }

    fseek(file, 0, SEEK_END);//Placer le pointeur de fichier � la fin du fichier
    int size = ftell(file);//Calculer la taille en octet du  ficher de lecture
    fseek(file, 0, SEEK_SET);//Replacer la position du pointeur de fichier au debut
    //Allou� de l'espace nexessaire pour contenir tous les caract�res du fichier
    char *buf = (char*) malloc(sizeof(char) * size + 1);//Allou� de l'espace necessaire pour stocker le fichier
    //Lire tous le fichier et le remplir dans le buffer
    fread(buf, 1, size, file);
    fclose(file);
    //Remplacer la fin du fichier par le caract�re fin de la chaine
    buf[size] = '\0';


    return XML_load(doc,buf);

}

void XMLDocument_free(XMLDocument* doc)
{
    XMLNode_free(doc->root);
}


#endif 
