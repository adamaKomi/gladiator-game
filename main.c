#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "header.h"
#define Talloc(x) (x *)malloc(sizeof(x))
#define NUM_FRAMES 12





typedef struct
{
 int height; //longueur
 int width; //largeur
}Taille;


typedef struct
{
  char titre[15]; //titre du widget
  GtkWidget *window; //la fenetre
  char icon[15]; //chemin de l'icone de la fenetre gboolean iconexist;//Booleen qui indique si la fenetre a une icone ou pas(1 oui 0 non)
  Taille dim; //dimension de la fenetre
}fenetre;

/*
* structure d'un bouton
*/
typedef struct
{
  char titre[15]; //titre du widget
  GtkWidget *buton; // le bouton
  char type; // 1 :normale/2 :radio/3 :check/4 :spin
  gboolean active;//TRUE active FALSE desactive
}Bouton;


//Structure d'une liste deroulante
typedef struct
{
    GtkWidget *Liste;
    char id[15];
}ListeDeroulant;



// Structure de l'image
typedef struct
{
    GtkWidget *image; // Widget d'image
    char path[20];  // Chemin vers le fichier d'image
    Taille dim;//   Taille de l'image
} MyImage;


//--------------------------------------BOX--------------------------------------------//
typedef struct
 {
    GtkWidget *box; //initialisation de widget box;
    gchar *orient;  //l'orientation dans la fenetre {horizontal,vertical};
}Box;

//-------------------------------------LABEL-------------------------------------------//

typedef struct
{
    GtkWidget* label;  //initialisation de widget label;
    gchar *text;       //le texte a ecrire;
}Label;


///--------------------------------------GRID--------------------------------------------//

typedef struct
{
 GtkWidget *grid;            //pointeur sur le grid
}Mongrid;


///steucture de positionnement
typedef struct
{
 double X;///Sur l'axe des absisses
 double Y;///Sur l'axe des ordonnée
}Dep;

///Structure pour les objets du jeu
typedef struct
{
 Dep *position;///Position des objets
 GdkPixbuf *image;///Image objet
 GdkPixbuf *scaled;///Image objet
}Objet;

///Les Objets
Objet *Tab[3];



///Structure de données pour un gladiateur
typedef struct
{
   int nuGlad;///Numero Gladiateur
   int nvforce;///Niveau de force
   int nvVie;///Niveau de vie
   Dep *Pos;///Position du gladiateur
   Dep *Vitesse;///Vitesse du gladiateur
   gboolean Agressif ;///TRUE:Agressif ; FALSE:Gentil //Agressif du Gladiateur
   char numArme;///Arme du gladiateur
   char Armure;///Armure du gladiateur
   gboolean bouclier;///Bouclier
    int nvExp;///Niveau d'experience en fonction des combats
    gboolean isControlled;///Qui le controle (user ou automatique)
    gboolean isfighting;///En combat
    gboolean iswalking;///Entrain de marché
    gboolean isattaking;///Attaque ou defense
    int nbCgagne;///Nombre de combat gagné
    int current_frame;                   /// Indice de l'image de saut actuelle
    gboolean isMovingRight; ///pour savoir le sens de direction
    gboolean hidden;///Montre si le gladiateur est caché
    int nbhid;///Compteur pour cacher le gladiateur
    int hidway;///Endroit ou est caché le gladiateur


}Gladiateur;

typedef struct
{
    GdkPixbuf *FramesR[12];
    GdkPixbuf *FramesL[12];
    GdkPixbuf *FramesAR[12];
    GdkPixbuf *FramesAL[12];
}Fram;

Fram *TabFr[13];
GdkPixbuf *Vie[7];




///Liste des gladiateurs
GList *gladiators = NULL;

///Gladiateur contrôlé par l'utilisateur (NULL s'il n'y en a pas)
Gladiateur *cont_gladiator = NULL;


typedef struct
{
     fenetre *F;
     Bouton *B1;
     Bouton *B2;
     MyImage *Image;
     Box *Boxe;
     Mongrid *grid;
     Label *lab;
     ListeDeroulant *L;

      Gladiateur *G;
      gboolean combat;
      int choix;
      gboolean is_paused;
      /// Dimensions de l'arène
       int ARENA_WIDTH;
       int ARENA_HEIGHT;
       GtkWidget *arena;
       GdkPixbuf *image;
       GdkPixbuf *scaled;///Image objet
       char Arenabac[30];

}Interface;



MyImage *init_image( char path[50], int width, int height)
{
    ///allocation d'espace
    MyImage *info =Talloc(MyImage);
    if( !info  )
    {
        g_print("\nErreur allocation : image");
        exit(-1);
    }
    ///remplir les champs
    strcpy( info->path, path );
    info->dim.width = width;
    info->dim.height = height;
    return(MyImage *)info;
}

void creer_image(MyImage *I)
{
    GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(I->path, NULL);
    GdkPixbuf *resized_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf,I->dim.width,I->dim.height, GDK_INTERP_BILINEAR);
    g_object_unref(src_pixbuf);
    I->image= gtk_image_new_from_pixbuf(resized_pixbuf);

}

/***************************************************************
Fonction: TesterAllocation: une fonction definie qui permet de tester
          si l'allocation d'un element a reussi. Le principe est de
          pouvoir passer n'importe quel type d'element en parametre
Entree: elem: L'element dont on veut tester
Sortie: affiche un message d'echec si l'allacation echoue
*******************************************************************/

 void TesterAllocation(elem)
{
    if (!elem) {
        printf("\nErreur d'allocation");
        exit(-1);
    }
}

void liberer_gladiateur(Gladiateur *gladiator)
{
    if (gladiator == NULL)
    {
        return;
    }

    /// Libérer les ressources spécifiques du gladiateur
    if (gladiator->Vitesse)
    {
        free(gladiator->Vitesse);
    }
    if (gladiator->Pos)
    {
        free(gladiator->Pos);
    }

    /// Libérer la structure du gladiateur elle-même
    free(gladiator);
}

void new_game(GtkWidget *widget, gpointer user_data)
{
    Interface *interface = (Interface *)user_data;
    GList *iter = gladiators;

    /// Libérer chaque gladiateur dans la liste
    while (iter != NULL)
    {
        Gladiateur *gladiator = (Gladiateur *)iter->data;
        iter = g_list_next(iter); /// Avancer l'itérateur avant de supprimer l'élément
        liberer_gladiateur(gladiator);
    }

    /// Libérer la liste elle-même
    g_list_free(gladiators);
    gladiators = NULL;
    interface->combat=FALSE;
    /// Redessiner l'arène après la suppression des gladiateurs
    gtk_widget_queue_draw(interface->arena);
}


void on_pause_clicked(GtkWidget *widget, gpointer user_data)
{
    Interface *interface = (Interface *)user_data;
   interface->is_paused = !interface->is_paused;

    if (interface->is_paused)
    {
        gtk_button_set_label(GTK_BUTTON(widget), "Resume");
    } else
    {
        gtk_button_set_label(GTK_BUTTON(widget), "Pause");
    }
}

fenetre *init_fenetre(char titre[15],char icon[15],int w,int h)
{

    ///Allocation de l'espace memoire
    fenetre *Nf=Talloc(fenetre);
    ///si l'allocation echoue
    TesterAllocation(Nf); ///quitter

    ///Remplir les champs
    strcpy(Nf->titre,titre);//titre de la fenetre
    strcpy(Nf->icon,icon);// icon sous forme de chemin
    Nf->dim.height=h;
    Nf->dim.width=w;

    return(fenetre*)Nf;

}

void parsefile(const char *filename,Interface *I)
{

    XMLDocument doc;
    int i;
    XMLNode *child;

    if (XMLDOC_Loading(&doc, filename))
    {
        XMLNode *root = XMLNode_child(doc.root, 0);///Recup racine
        if ( !strcmp((char*)root->tag, "arena"))
        {
            if(xmlGetProp(root, (char*)"width"))
                I->ARENA_WIDTH=  atoi((char*)xmlGetProp(root, (char*)"width"));

            if(xmlGetProp(root, (char*)"height") )
                I->ARENA_HEIGHT=  atoi((char*)xmlGetProp(root, (char*)"height"));

            if(xmlGetProp(root,(char*)"background"))
                 strcpy(I->Arenabac,(char*) xmlGetProp(root,(char*)"background"));

            if(xmlGetProp(root, (char*)"Combat"))
            {
               if( !strcmp("TRUE",(char *)xmlGetProp(root,(char*)"Combat")) )
                  I->combat=TRUE;
               else
                   I->combat=FALSE;
            }

             if(xmlGetProp(root, (char*)"is_paused"))
              {
                if( !strcmp("TRUE",(char*)xmlGetProp(root,(char*)"is_paused")) )
                    I->is_paused=TRUE;
                else
                    I->is_paused=FALSE;
              }

           for(i=0;i<root->children.size;i++)
           {
               ///Parcourir tous les fils du la fenêtre et les creer
                    child = XMLNode_child(root,i);
                 if (!strcmp((char*)child->tag,  "gladiator"))
                 {

                    Gladiateur *gladiator = Talloc(Gladiateur);
                    TesterAllocation(gladiator);
                    gladiator->Pos = Talloc(Dep);
                    TesterAllocation(gladiator->Pos);
                    gladiator->Vitesse = Talloc(Dep);
                    TesterAllocation(gladiator->Vitesse);


                     if(xmlGetProp(child, (char*)"PosX"))
                         gladiator->Pos->X = atof((char*) xmlGetProp(child, (char*)"PosX"));

                     if(xmlGetProp(child, "PosY"))
                         gladiator->Pos->Y = atof((char *) xmlGetProp(child, (char*) "PosY"));

                     if(xmlGetProp(child, "VitesseX"))
                        gladiator->Vitesse->X = atof((char *) xmlGetProp(child, (char*)"VitesseX"));

                     if(xmlGetProp(child, "VitesseY"))
                        gladiator->Vitesse->Y = atof((char *) xmlGetProp(child,(char*)"VitesseY"));

                     if (xmlGetProp(child, "nvVie"))
                        gladiator->nvVie = atoi((char *) xmlGetProp(child,(char*)"nvVie"));

                     if (xmlGetProp(child, "nvforce"))
                         gladiator->nvforce = atoi((char *) xmlGetProp(child, (char*)"nvforce"));

                     if (xmlGetProp(child, "nvExp"))
                         gladiator->nvExp = atoi((char *) xmlGetProp(child, (char*)"nvExp"));

                     if (xmlGetProp(child, "nbCgagne"))
                        gladiator->nbCgagne = atoi((char *) xmlGetProp(child, (char*)"nbCgagne"));

                     if (xmlGetProp(child, "nuGlad"))
                         gladiator->nuGlad = atoi((char *) xmlGetProp(child, (char*)"nuGlad"));

                     if (xmlGetProp(child, "hidway"))
                         gladiator->hidway = atoi((char *) xmlGetProp(child,(char*)"hidway"));

                     if (xmlGetProp(child, "current_frame"))
                         gladiator->current_frame = atoi((char *) xmlGetProp(child,(char*) "current_frame"));

                     if (xmlGetProp(child, "nbhid"))
                         gladiator->nbhid = atoi((char *) xmlGetProp(child, (char*)"nbhid"));


                     if (xmlGetProp(child, "numArme"))
                         gladiator->numArme=(char *)xmlGetProp(child,(char*)"numArme");


                     if (xmlGetProp(child, "armure"))
                         gladiator->Armure=(char *) xmlGetProp(child, (char*) "armure");

                     if (xmlGetProp(child, (char*) "isControlled") )
                     {
                         if (!strcmp("TRUE", (char *) xmlGetProp(child, (char*) "isControlled")))
                             gladiator->isControlled = TRUE;
                         else
                            gladiator->isControlled = FALSE;
                     }

                    if (xmlGetProp(child, (char*) "isMovingRight") )
                    {
                        if (!strcmp("TRUE", (char *) xmlGetProp(child, (char*)"isMovingRight")))
                            gladiator->isMovingRight = TRUE;
                        else
                            gladiator->isMovingRight = FALSE;
                    }
                    if (xmlGetProp(child, (char*)"isfighting"))
                    {
                        if (!strcmp("TRUE", (char *) xmlGetProp(child,(char*)"isfighting")))
                            gladiator->isfighting = TRUE;
                        else
                            gladiator->isfighting = FALSE;
                    }

                    if (xmlGetProp(child, (char*)"iswalking") )
                    {
                        if (!strcmp("TRUE", (char *) xmlGetProp(child, (char*) "iswalking")))
                            gladiator->iswalking = TRUE;
                        else
                            gladiator->iswalking = FALSE;
                    }

                    if (xmlGetProp(child,(char*) "isattaking"))
                    {
                        if (!strcmp("TRUE", (char *) xmlGetProp(child,(char*) "isattaking")))
                            gladiator->isattaking = TRUE;
                        else
                            gladiator->isattaking = FALSE;
                    }

                     if (xmlGetProp(child, (char*)"Agressif") )
                     {
                         if (!strcmp("TRUE", (char *) xmlGetProp(child,(char*) "Agressif")))
                             gladiator->Agressif = TRUE;
                         else
                             gladiator->Agressif = FALSE;
                     }

                     if (xmlGetProp(child,(char*) "bouclier") )
                     {
                         if (!strcmp("TRUE", (char *) xmlGetProp(child, (char*) "bouclier")))
                             gladiator->bouclier = TRUE;
                         else
                             gladiator->bouclier = FALSE;
                     }

                     if (xmlGetProp(child,(char*) "hidden") )
                     {
                         if (!strcmp("TRUE", (char *) xmlGetProp(child, (char*) "hidden")))
                             gladiator->hidden = TRUE;
                         else
                             gladiator->hidden = FALSE;
                     }


                     gladiators = g_list_append(gladiators, gladiator);
                 }
            }

        }
        else g_print("arena not found");
            XMLDocument_free(&doc);

    }
    else
        g_warning("Failed to parse XML file %s", filename);
    return ;
}
void load_game_state(GtkWidget *w, gpointer data)
{
    GList *iter = gladiators;
    /// Libérer chaque gladiateur dans la liste
    while (iter != NULL)
    {
        Gladiateur *gladiator = (Gladiateur *)iter->data;
        iter = g_list_next(iter); /// Avancer l'itérateur avant de supprimer l'élément
        liberer_gladiateur(gladiator);
    }

    /// Libérer la liste elle-même
    g_list_free(gladiators);

    gladiators = NULL;
     Interface *I=(Interface *)data;
   parsefile("save.xml",I);

}

void save_game(GtkWidget *w, gpointer data)
{
    Interface *I=(Interface*)data;
    FILE *file = fopen("save.xml", "w");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file for writing: %s\n", "save.xml");
        return;
    }


    /// Ouvrir la balise root
    fprintf(file, "<arena width=\"%d\" height=\"%d\" Combat=\"%s\" is_paused=\"%s\" background=\"%s\">\n"
            ,I->ARENA_WIDTH,I->ARENA_HEIGHT,I->combat? "TRUE":"FALSE",I->is_paused? "TRUE":"FALSE",I->Arenabac);

    /// Parcourir la liste des gladiateurs et écrire leurs informations
    for (GList *iter = gladiators; iter != NULL; iter = g_list_next(iter))
    {
        Gladiateur *gladiator = (Gladiateur *)iter->data;
fprintf(file, " <gladiator PosX=\"%.2f\" PosY=\"%.2f\" VitesseX=\"%.2f\" VitesseY=\"%.2f\" nuGlad=\"%d\" numArme=\"%c\" Armure=\"%c\" nvVie=\"%d\" nvforce=\"%d\" nvExp=\"%d\" nbCgagne=\"%d\" current_frame=\"%d\" nbhid=\"%d\" hidway=\"%d\" isControlled=\"%s\" isMovingRight=\"%s\" isfighting=\"%s\" iswalking=\"%s\" isattaking=\"%s\" Agressif=\"%s\" bouclier=\"%s\" hidden=\"%s\"/>\n",
                gladiator->Pos->X, gladiator->Pos->Y,gladiator->Vitesse->X,gladiator->Vitesse->Y,gladiator->nuGlad,gladiator->numArme,gladiator->Armure, gladiator->nvVie, gladiator->nvforce, gladiator->nvExp, gladiator->nbCgagne,gladiator->current_frame,gladiator->nbhid,gladiator->hidden? gladiator->hidway:-1,
                gladiator->isControlled ? "TRUE" : "FALSE", gladiator->isMovingRight?  "TRUE" : "FALSE",gladiator->isfighting ? "TRUE" : "FALSE",gladiator->iswalking ?  "TRUE" : "FALSE",gladiator->isattaking ?  "TRUE" : "FALSE",gladiator->Agressif?  "TRUE" : "FALSE",gladiator->bouclier?  "TRUE" : "FALSE",gladiator->hidden?  "TRUE" : "FALSE");
    }

    /// Fermer la balise root
    fprintf(file, "</arena>");

    fclose(file);
}



void toggle_maximize(GtkButton *button, gpointer data)
{
    GtkWindow *window = GTK_WINDOW(data);
    if (gtk_window_is_maximized(window))
        gtk_window_unmaximize(window);
    else
        gtk_window_maximize(window);

}

void on_reduce_button_clicked(GtkButton *button, gpointer user_data)
{
    /// Recuperer la fenetre associee au widget button
    GtkWidget *window = gtk_widget_get_toplevel(GTK_WIDGET(user_data));

    /// Verifier que window est une fenetre valide
    if (GTK_IS_WINDOW(window))
        /// Reduire la fenetre
        gtk_window_iconify(GTK_WINDOW(window));
     else
        /// Gerer le cas oe window n'est pas une fenetre valide
        g_warning("Le widget associe au bouton n'est pas une fenetre valide.");

}


void creer_fenetre(Interface *I,gboolean edit)
{

      GtkWidget *button;
     MyImage *im=init_image(I->F->icon,50,50);
    ///Si le type est Toplevel
     I->F->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
     ///Mettre la taille a la fenetre
     gtk_window_set_default_size(GTK_WINDOW(I->F->window),I->F->dim.width,I->F->dim.height);

        /// Creez une barre de titre personnalisee
        GtkHeaderBar *header_bar = GTK_HEADER_BAR(gtk_header_bar_new());
        GtkWidget *box=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
        gtk_box_set_homogeneous(box,FALSE);
        gtk_container_add(GTK_CONTAINER(header_bar),box);

          creer_image(im);
         gtk_container_add(GTK_CONTAINER(box),im->image);

        GtkWidget *title_label = gtk_label_new(I->F->titre);

        gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header_bar), title_label); /// Set the label as the custom title


       /// Ajoutez un bouton de fermeture a la barre de titre
            GtkWidget *close_button = gtk_button_new_with_label("X");
             gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar),close_button);
            g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_widget_destroy),I->F->window);

         /// Ajoutez un bouton d'agrandissement a la barre de titre

            GtkWidget *maximize_button = gtk_button_new_with_label("[]");
             gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), maximize_button );
            /// Connectez le signal "clicked" du bouton d'agrandissement a une fonction de gestion personnalisee
            g_signal_connect(maximize_button, "clicked", G_CALLBACK(toggle_maximize), I->F->window);

        /// Ajoutez un bouton de reduction a la barre de titre
            GtkWidget *reduce_button = gtk_button_new_with_label("-");
             gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), reduce_button);
           ///Connectez le signal "clicked" du bouton de reduction a la fonction de reduction de la fenetre
            g_signal_connect(reduce_button, "clicked", G_CALLBACK(on_reduce_button_clicked),GTK_WINDOW(I->F->window));

            if(edit)
            {
                 /// Ajoutez un bouton de jeu
                button = gtk_button_new_with_label("Nouveau jeu");
                gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar),button);
                g_signal_connect(button, "clicked", G_CALLBACK(new_game), I);

              button = gtk_button_new_with_label("Pause");
              g_signal_connect(button, "clicked", G_CALLBACK(on_pause_clicked), I);
              gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar),button);

                button = gtk_button_new_with_label("Save");
                g_signal_connect(button, "clicked", G_CALLBACK(save_game), I);
                gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar),button);

                button = gtk_button_new_with_label("Charger");
                g_signal_connect(button, "clicked", G_CALLBACK(load_game_state), I);
               gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar),button);
            }



         ///Attachez la barre de titre a la fenetre
        gtk_window_set_titlebar(GTK_WINDOW(I->F->window), GTK_WIDGET(header_bar));

        gtk_window_set_position(GTK_WINDOW(I->F->window),GTK_WIN_POS_CENTER);

        g_signal_connect(I->F->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

}



Mongrid* Init_grid()
{
	Mongrid *g;

	///allocation d'espace memoire pour  la structure
	g =Talloc(Mongrid);
	if(!g)
	{
		printf("\n erreur d'allocation[grid]");
		exit(0);
	}

	g->grid = gtk_grid_new();
	return(Mongrid*)g;
}


ListeDeroulant *init_Liste(char ID[30])
{
    ListeDeroulant *L=(ListeDeroulant*)malloc(sizeof(ListeDeroulant));
    if(!L) exit(-1);
    strcpy(L->id,ID);
    return(ListeDeroulant*)L;
}

Interface *combo_box_changed(GtkComboBox *combobox, gpointer user_data)
{
    /// Recuperer la structure de l'interface
    Interface *interface = (Interface *)user_data;
    Gladiateur *D=interface->G;
    gchar *selected_item = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox));

    return(Interface*)interface;
}
void Add_ListeElm(ListeDeroulant *L, char content[30])
{
     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(L->Liste),content);
}

void creer_Liste(ListeDeroulant *L)
{
     ///Creation des listes deroulantes
     L->Liste =gtk_combo_box_text_new();

}


void Ajouter_elem_grid(Mongrid *g, GtkWidget* elem,int x,int y)
{
	///ajouter l'elem child dans le grid selon les coordonnees et la taille du grid
	gtk_grid_attach(GTK_GRID(g->grid), elem, x,y, 1,1);
}


Label *initLabel(Label *label,char *text)
{
    label = Talloc(Label);
    TesterAllocation(label);
    label->text =g_locale_to_utf8(text,
                                              -1,
                                             NULL,
                                             NULL,
                                             NULL);
    return label;
}


void CreerLabel(Label *label)
{
    label->label = gtk_label_new(label->text);

}///fin de la fonction;


Bouton *init_bouton(char Label[30],gboolean active,char type)
{
   /// Alloction de l'espace memoire
    Bouton *NB=Talloc(Bouton);
   ///Si l'allocation echoue quitter
    TesterAllocation(NB);
///Sinon remplir les champs
 strcpy(NB->titre,Label);
 NB->active=active;
 NB->type=type;
 return(Bouton*)NB;

}
/// Fonction de rappel pour le signal "clicked" d'un bouton radio
Interface *radio_button_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
     /// Recuperer la structure de l'interface
    Interface *interface = (Interface *)user_data;
    Gladiateur *D=interface->G;
    gboolean active = gtk_toggle_button_get_active(togglebutton);
    if (active)
    {
        gchar *label = gtk_button_get_label(GTK_BUTTON(togglebutton));

        if(!strcmp("Agressif",label))
           D->Agressif=TRUE;
        else
          if(!strcmp("Gentil",label))
           D->Agressif=FALSE;

    }

    return(Interface*)interface;
}


void Value_SpineBoutton(GtkButton *button, gpointer user_data)
{
    /// Recuperer la structure de l'interface
    Interface *interface = (Interface *)user_data;
    Gladiateur *D=interface->G;
     if (GTK_IS_SPIN_BUTTON(button))
    {
        int value = gtk_spin_button_get_value(button);
         D->nvforce=value;
    }
    else
    {
        g_print("Erreur : objet invalide passe\n");
    }
}


void creer_button_RadioPere(Interface *I)
{

    I->B1->buton=gtk_radio_button_new_with_label(NULL,I->B1->titre);
    gtk_widget_set_sensitive(I->B1->buton, TRUE);
    gtk_toggle_button_set_active(GTK_RADIO_BUTTON(I->B1->buton),I->B1->active);
    g_signal_connect(I->B1->buton, "toggled", G_CALLBACK(radio_button_toggled),I);

}


void creer_button_radiofils(Interface *I,GtkWidget *pere)
{
        I->B2->buton=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(pere),I->B2->titre);
        gtk_widget_set_sensitive(I->B2->buton, TRUE);
        ///Activez le bouton
        gtk_toggle_button_set_active(GTK_RADIO_BUTTON(I->B2->buton),I->B2->active);
        g_signal_connect(I->B2->buton, "toggled", G_CALLBACK(radio_button_toggled),I);


}

Box *init_box(Box* box,char opt)
{

    ///l'allocation memoire de l'or du box;
    box =Talloc(Box);
    if(!box)
    {
        printf("\nErreur d'allocation memoire du BOX!!");  ///test d'allocation memoire
        exit(-1);
    }
    ///l'allocation memoire de l'orientation du box;
    if(opt=='0')
    box->orient = g_strdup("horizontal");
    else
    box->orient= g_strdup("vertical");
    return box;
}

void creer_Box(Box *box)
{

    if(strcmp(box->orient,"vertical")==0)box->box = gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
    else if(strcmp(box->orient,"horizontal")==0)box->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
}


void creer_button(Interface *I)
{
    GtkWidget *img;

    switch(I->B1->type)
    {
      ///Cas d'un bouton normal
       case '1':
            I->B1->buton=gtk_button_new_with_label(I->B1->titre);

          break;
        ///Cas d'un check_box
    case '3':
        {
               I->B1->buton=gtk_check_button_new_with_label(I->B1->titre);
                 ///Activez le bouton
                 gtk_toggle_button_set_active(GTK_CHECK_BUTTON(I->B1->buton),I->B1->active);
        }
        break;
        ///Cas d'un bouton spin
    case '4':
        {
            I->B1->buton=gtk_spin_button_new_with_range(0,100,1); //(val Min,Val Max,ecart)
            g_signal_connect(I->B1->buton, "value-changed", G_CALLBACK(Value_SpineBoutton), I);
        }
        break;
    }


}


gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    Interface *interface = (Interface *)user_data;
     GdkPixbuf *pixbuf;
     /// Dessiner l'image de fond de l'arène
     interface->image= gdk_pixbuf_new_from_file("bac1.jpg", NULL);
     interface->scaled= gdk_pixbuf_scale_simple(interface->image, interface->ARENA_WIDTH, interface->ARENA_HEIGHT, GDK_INTERP_BILINEAR);

    gdk_cairo_set_source_pixbuf(cr,GDK_PIXBUF(interface->scaled), 0, 0);
    cairo_paint(cr);
    g_object_unref(GDK_PIXBUF(interface->image));
    g_object_unref(GDK_PIXBUF(interface->scaled));

    /// Dessiner les objets dans l'arène
    for (int i = 0; i < 3; i++)
    {
        gdk_cairo_set_source_pixbuf(cr, Tab[i]->scaled, Tab[i]->position->X, Tab[i]->position->Y);
        cairo_paint(cr);
    }


    /// Dessiner les gladiateurs avec animation
    for (GList *iter = gladiators; iter != NULL; iter = g_list_next(iter))
    {

        Gladiateur *gladiator = (Gladiateur *)iter->data;

        if(!gladiator->hidden)
        {
            if(interface->is_paused)
              gladiator->current_frame=0;
            if(gladiator->iswalking)
            {

                if(gladiator->isMovingRight)
                    pixbuf = TabFr[gladiator->nuGlad]->FramesR[gladiator->current_frame];
                else
                    pixbuf =TabFr[gladiator->nuGlad]->FramesL[gladiator->current_frame];

           }
           else
            {///Chargement des frames de combat

                if(gladiator->isMovingRight)
                    pixbuf =TabFr[gladiator->nuGlad]->FramesAR[gladiator->current_frame];
                else
                    pixbuf =TabFr[gladiator->nuGlad]->FramesAL[gladiator->current_frame];
         }

        gdk_cairo_set_source_pixbuf(cr, pixbuf, gladiator->Pos->X, gladiator->Pos->Y);
        cairo_paint(cr);
        ///Gerer le niveau de vie
        if((gladiator->nvVie>=0) && (gladiator->nvVie<200))
            pixbuf=Vie[0];///0-19%
        else
           if((gladiator->nvVie>=200) && (gladiator->nvVie<400))
                 pixbuf=Vie[1];///20-39%
           else
              if((gladiator->nvVie>=400) && (gladiator->nvVie<600))
                 pixbuf=Vie[2];///40-59%
              else
                   if((gladiator->nvVie>=600) && (gladiator->nvVie<800))
                       pixbuf=Vie[3];///60-79%
                    else
                        if((gladiator->nvVie>=800) && (gladiator->nvVie<1000))
                            pixbuf=Vie[4];///80-99%
                        else
                         pixbuf=Vie[5];///100%
        ///Dessiner le niveau de vie
        gdk_cairo_set_source_pixbuf(cr, pixbuf, gladiator->Pos->X, gladiator->Pos->Y);
        cairo_paint(cr);
        /// Incrémenter le frame actuel du gladiateur pour l'animation
        gladiator->current_frame = (gladiator->current_frame + 1) % NUM_FRAMES;
        }
        else
        {
            if(!interface->is_paused)
            {
                 ///Le cacher durant 25 Mise à jour
              if(gladiator->nbhid<25)
                gladiator->nbhid++;
              else
              {
                gladiator->current_frame=0;
                gladiator->hidden=FALSE;
                gladiator->nbhid=0;
                switch(gladiator->hidway)
                {
                    case 0: gladiator->Pos->X=Tab[0]->position->X+100;
                            gladiator->Pos->Y=Tab[0]->position->Y+60;
                            gladiator->Vitesse->X = -gladiator->Vitesse->X;
                            // Changer l'animation
                             gladiator->isMovingRight = !gladiator->isMovingRight;

                            break;
                    case 1:gladiator->Pos->X=Tab[1]->position->X+100;
                            gladiator->Pos->Y=Tab[1]->position->Y+100;
                            break;
                    default: gladiator->Pos->X=Tab[2]->position->X+100;
                             gladiator->Pos->Y=Tab[2]->position->Y-100;
                             gladiator->Vitesse->X = -gladiator->Vitesse->X;
                            // Changer l'animation
                             gladiator->isMovingRight = !gladiator->isMovingRight;

                }


            }
        }
    }


  }


    return FALSE;
}


/*********************************************************************************
Fonction: on_mouse_click:Fonction pour gerer les clics de la souris
Entrée:GtkWidget *widget:l'élément qui resposable du signal
        GdkEventButton *event:la position de l'évènement où le signal est emis
        gpointer user_datar:Données passées en paramètre dans la fonction signal
Sortie: la fonction retourne TRUE à la fin du traitement
***********************************************************************************/
gboolean on_mouse_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    if (event->button == GDK_BUTTON_PRIMARY)
    { /// Bouton gauche de la souris
        /// Parcourir la liste des gladiateurs pour verifier si un gladiateur a été cliqué
        for (GList *iter = gladiators; iter != NULL; iter = g_list_next(iter))
         {
                ///Recuperer la structure du gladiateur
                Gladiateur *gladiator = (Gladiateur*)iter->data;
                ///Verifier si le clic a été effectué tous proche du gladiateur
            if( (event->x >= gladiator->Pos->X) && (event->x <= gladiator->Pos->X + 90) &&
                (event->y >= gladiator->Pos->Y) && (event->y <= gladiator->Pos->Y + 90)
              )
            {
                /***Si on a clic proche d'un gladiateur
                 Si aucun gladiateur n'est pas selectionné*/
                if (!cont_gladiator)
                {
                    /// attribuer ce gladiateur à l'utilisateur pour le contrôler
                    cont_gladiator = gladiator;
                    gladiator->isControlled= TRUE;
                    return TRUE;
                }
                else
                {
                    /// Si un gladiateur est déjà selectionné, le deselectionner
                    cont_gladiator->isControlled= FALSE;
                    cont_gladiator = NULL;
                    return TRUE;
                }
            }
        }
    }
    return TRUE;
}
/*********************************************************************************
Fonction:on_mouse_motion:Fonction pour gérer les evenements de mouvement de la souris
Entrée:GtkWidget *widget:l'élément qui resposable du signal
       GdkEventMotion *event:la position de l'évènement où le signal est emis
       gpointer user_datar:Données passées en paramètre dans la fonction signal
Sortie: la fonction retourne TRUE à la fin du traitement
***********************************************************************************/
gboolean on_mouse_motion(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
    ///Recuperer les données de l'interface
    Interface *I=(Interface*)user_data;
    ///Si on a un gladiateur qui est contrôlé
  if (cont_gladiator)
  {
    ///Faire suivre le gladiateur avec les mouvements du curseur
    cont_gladiator->Pos->X = event->x - 90;
    cont_gladiator->Pos->Y= event->y - 90;
    /// Limiter le deplacement du gladiateur à l'interieur de l'arène
    if (cont_gladiator->Pos->X < 0)  cont_gladiator->Pos->X = 0;
    if (cont_gladiator->Pos->X > I->ARENA_WIDTH - 150) cont_gladiator->Pos->X = I->ARENA_WIDTH - 150;
    if (cont_gladiator->Pos->Y < 70) cont_gladiator->Pos->Y = 70;
    if (cont_gladiator->Pos->Y > I->ARENA_HEIGHT - 150) cont_gladiator->Pos->Y = I->ARENA_HEIGHT - 150;
     /// Redessiner l'arène pour afficher les nouveaux emplacements des gladiateurs
    gtk_widget_queue_draw(widget);
 }
    return TRUE;
}

/*********************************************************************************
Fonction:calculate_Exp:Fonction pour calculer l'experience gagner par le gladiateur
Entrée:int nbcombat:Répresente le nombre de combat gagner par le gladiateur
Sortie: la fonction retourne la valeur de l'expeience en entier
***********************************************************************************/
int calculate_Exp(int nbcombat)
{
    return (int)nbcombat*5;
}
/*********************************************************************************
Fonction:Calculate_Protect:Fonction pour calculer la protection du gladiateur
Entrée:char armure:le type de l'armure
       gboolean bouclier:la presence ou l'absence du bouclier
Sortie:la fonction retourne la valeur de la protection en entier
***********************************************************************************/
int Calculate_Protect(char armure,gboolean bouclier)
{
    switch(armure)
    {
       case 'F' :return bouclier? 4:5; ///Armure en fer
       case 'B' :return bouclier? 6:7; ///Armure en Bronze
       case 'O': return bouclier? 8:9; ///Armure en Or
    }
}
/*********************************************************************************
Fonction:calculate_damage:Fonction pour calculer les domages reçu par un gladiateur
Entrée:int force:Force du gladiateur
        int nvExp:Experience du gladiateur
        char arme:type de l'arme
Sortie:la fonction retourne la valeur de domage en entier
***********************************************************************************/
int calculate_damage(int force, int nvExp, char arme)
{
  float a,b;///Variables de calcul
  b=(float)(force+nvExp)/100;///ratio de la force et de l'experience

    switch (arme)
    {
        case 'S':/// Si l'arme est un Sabre
            a=(float)7 / 20;///ratio du sabre
            a=(a+b)*10;
            return (int)a;
        case 'H':///Si l'arme est une Hache
            a=(float)15 / 20;///ratio de la hache
            a=(a+b)*10;
            return (int)a;
        case 'C':///Si l'arme est une Chaine
            a=(float)14/ 20;///ratio de la chaine
            a=(a+b)*10;
            return (int)a;
        case 'M':///Si l'arme est un Marteau
            a=(float)18 / 20;///ratio du Marteau
            a=(a+b)*10;
            return (int)a;
        case 'D':///Si l'arme est une Dague
            a=(float)12 / 20;///ratio d'une dague
            a=(a+b)*10;
            return (int)a;
        default:
            return 0;
    }
}
/*********************************************************************************
Fonction:Mise_enCombat:Fonction pour actualisé l'état d'un gladiateur
Entrée:Gladiateur *G:gladiateur
Sortie:la fonction met à jour le nouvel état du gladiateur
***********************************************************************************/
void Mise_enCombat(Gladiateur *G)
{
    G->isfighting=TRUE;///Montrer qu'il est en combat
    G->iswalking=FALSE;///Monter qu'il n'effectue plus de changement de position
    G->current_frame=0;///Remettre ses frames à Zero
    G->isattaking=TRUE;///Mettre sa position en attaque pour l'utilisation des frames
    ///Annuler sa vitesse
    G->Vitesse->X=0.0;
    G->Vitesse->Y=0.0;
}

/*********************************************************************************
Fonction:MINI:Fonction qui retourne le minimum entre 2 valeurs
Entrée:int x : valeur de comparaison
       int y :valeur de comparaison
Sortie:la fonction retourne le plus petit
***********************************************************************************/
int MINI(int x,int y)
{
    return(x<y? x:y);
}
/*********************************************************************************
Fonction:Arret_Combat:Fonction pour actualisé l'état d'un gladiateur
Entrée:Gladiateur *G:gladiateur
Sortie:la fonction met à jour le nouvel état du gladiateur
***********************************************************************************/
void Arret_Combat(Gladiateur *G)
{
    G->isfighting=FALSE;///Arreter le combat
    G->iswalking=TRUE;///Le remettre en marche s'il n'est pas mort
}
/*********************************************************************************
Fonction:Vainqueur:Fonction pour actualisé l'état d'un gladiateur après une victoire
Entrée:Gladiateur *G:gladiateur
Sortie:la fonction met à jour le nouvel état du gladiateur
***********************************************************************************/
void Vainqueur(Gladiateur *G1,Gladiateur *G2)
{
    G1->nbCgagne++;///Augmenter son nombre de combat gagner
    G1->nvVie = 1000;///Remettre son niveau de vie
    G1->nvExp=calculate_Exp(G1->nbCgagne);///mettre à jour son experience
    G1->nvforce = MINI(G1->nvforce + (G2->nvforce / 2), 100);///Augmenter sa force
}

/*********************************************************************************
Fonction:CombatGlad:Foncntion pour le clacul des changements pendant le combat
Entrée:Gladiateur *G1:1er Gladiateur
       Gladiateur *G2:2e Gladiateur
       Interface *I :Interface de combat
Sortie:la fonction effectue les calculs des domages et reranche continuellement
        le niveau de vie des gladiateurs
***********************************************************************************/
gboolean CombatGlad(Gladiateur *G1, Gladiateur *G2, Interface *I)
{

    ///Calcul de la protection des gladiateurs
    int protect1 = Calculate_Protect(G1->Armure, G1->bouclier);
    int protect2 = Calculate_Protect(G2->Armure, G2->bouclier);

    ///Calcul des domages que les gladiateurs peuvent infliger
    int damage1 = calculate_damage(G1->nvforce, G1->nvExp, G1->numArme);
    int damage2 = calculate_damage(G2->nvforce, G2->nvExp, G2->numArme);
    ///Difference entre les domages et la protection
    damage2-=protect1;
    damage1-=protect2;


    ///Les positionner de manière opposé
    G1->isMovingRight = TRUE;
    G2->isMovingRight = FALSE;

    ///Si les gladiateurs sont toujours vivants
    if( (G1->nvVie > 0) && (G2->nvVie > 0) )
     {
         ///Soustraire les domages
        G1->nvVie -=damage2;
        G2->nvVie -=damage1;
        if(G1->nvVie > 0 && G2->nvVie > 0)
        return TRUE; /// Continue the combat
    }

    ///Si l'un est mort
    if (G1->nvVie > 0)
        Vainqueur(G1,G2);
    else
        Vainqueur(G2,G1);

    ///Arreter le combat
    Arret_Combat(G1);
    Arret_Combat(G2);

    return FALSE; /// End combat
}

/*********************************************************************************
Fonction:update_positions:Fonction de mise a jour des positions des gladiateurs
Entrée:Interface *I :Interface de combat
Sortie:la fonction effectue les calculs du positionnement  des gladiateurs
***********************************************************************************/
gboolean update_positions(Interface *I)
{
    /**Si l'interface est en pause ne pas
     changer la position des gladiateurs*/
    if(I->is_paused)
        return G_SOURCE_CONTINUE;

    /// Parcourir la liste des gladiateurs et mettre a jour leurs positions
    for (GList *iter1 = gladiators; iter1 != NULL; iter1 = g_list_next(iter1))
    {
        Gladiateur *gladiator1 = (Gladiateur *)iter1->data;
        /// Si le gladiateur est contrôlé par l'utilisateur, ne pas le deplacer automatiquement
      if(  (!gladiator1->isControlled) &&
            (!gladiator1->isfighting) &&
            (!gladiator1->hidden)
         )
         {
           /// Generer de nouveaux deplacements aléatoires
            /// Ajustement aleatoire de la vitesse horizontale
            gladiator1->Vitesse->X +=g_random_double_range(-0.1, 0.1);
            /// Ajustement aleatoire de la vitesse verticale
            gladiator1->Vitesse->Y += g_random_double_range(-0.1, 0.1);
            /// Limiter la vitesse maximale
            if ( gladiator1->Vitesse->X > 3.0 ) gladiator1->Vitesse->X  = 3.0;
            if ( gladiator1->Vitesse->Y > 3.0 ) gladiator1->Vitesse->Y = 3.0;
            /// Limiter la vitesse minimale
            if ( gladiator1->Vitesse->X  < -3.0 ) gladiator1->Vitesse->X = -3.0;
            if ( gladiator1->Vitesse->Y < -3.0 ) gladiator1->Vitesse->Y= -3.0;

            /// Appliquer les deplacements
            gladiator1->Pos->X += gladiator1->Vitesse->X;
            gladiator1->Pos->Y += gladiator1->Vitesse->Y;


            /**Verifier si le gladiateur atteint
            les bords de l'arene et inverser sa direction si necessaire*/
            if( (gladiator1->Pos->X < 0) ||
                (gladiator1->Pos->X> I->ARENA_WIDTH - 150)

              )
              {
                 gladiator1->Vitesse->X = -gladiator1->Vitesse->X;
                /// Changer l'animation
                gladiator1->isMovingRight = !gladiator1->isMovingRight;
              }

            if( (gladiator1->Pos->Y < 70 )||
                (gladiator1->Pos->Y > I->ARENA_HEIGHT - 150) ||
                (gladiator1->Pos->Y>=0 &&  gladiator1->Pos->Y<=300)
              )
                gladiator1->Vitesse->Y = -gladiator1->Vitesse->Y;
         }
    }

    return G_SOURCE_CONTINUE;
}

/*********************************************************************************
Fonction:CreeFrame:Fonction qui les frames des gladiateurs
Entrée:char ch1[30]:path des images
       char ch2[30]:path des images
        int T:Taille de l'image
Sortie:la fonction charge les images et les crée
***********************************************************************************/
Fram *CreeFrame(char ch1[100],char ch2[100],int T)
{
    ///Variables
    GdkPixbuf *image;
    Fram *F=Talloc(Fram);
    TesterAllocation(F);
    char filename[100];
    int i ;

    ///Recuperation des frames du deplacement droit
    for(i=0;i<12;i++)
    {
       sprintf(filename, "%s%dright.png",ch1,i);
       image= gdk_pixbuf_new_from_file(filename, NULL);
        if (image == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
    }
       /// Redimensionner l'image de l'objet à la taille souhaitée
       F->FramesR[i]= gdk_pixbuf_scale_simple(image,T, T, GDK_INTERP_BILINEAR);
    }
     ///Recuperation des frames du deplacement gauche
    for(i=0;i<12;i++)
    {
       sprintf(filename, "%s%dleft.png",ch1,i);
       image= gdk_pixbuf_new_from_file(filename, NULL);
        if (image == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
    }
      /// Redimensionner l'image de l'objet à la taille souhaitée
      F->FramesL[i]= gdk_pixbuf_scale_simple(image,T, T, GDK_INTERP_BILINEAR);
    }
     ///Recuperation des frames des attaques droit
    for(i=0;i<12;i++)
    {
      sprintf(filename, "%s%dright.png",ch2,i);
      image= gdk_pixbuf_new_from_file(filename, NULL);
       if (image == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
    }
      /// Redimensionner l'image de l'objet à la taille souhaitée
      F->FramesAR[i]= gdk_pixbuf_scale_simple(image,T, T, GDK_INTERP_BILINEAR);
    }
     ///Recuperation des frames des attaques gauche
    for(i=0;i<12;i++)
    {
        sprintf(filename, "%s%dleft.png",ch2,i);
        image= gdk_pixbuf_new_from_file(filename, NULL);
         if (image == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
    }
        /// Redimensionner l'image de l'objet à la taille souhaitée
        F->FramesAL[i]= gdk_pixbuf_scale_simple(image,T, T, GDK_INTERP_BILINEAR);
    }
    return(Fram*)F;
}

/*********************************************************************************
Fonction:initFrameGlad:Fonction qui charge les frames des gladiateurs
Entrée:void
Sortie:la fonction charge les paths pour la création des frames
***********************************************************************************/
void initFrameGlad()
{
    ///Variables
    GdkPixbuf *image;
    int i;
    char filename[30];
    ///Chargement des differentes paths
    TabFr[0]=CreeFrame("G0/run/run_","G0/Attack/at_",120);
    TabFr[1]=CreeFrame("G1/run/run_","G1/Attack/at_",110);
    TabFr[2]=CreeFrame("G2/run/run_","G2/Attack/at_",180);
    TabFr[3]=CreeFrame("G3/run/run_","G3/Attack/at_",180);
    TabFr[4]=CreeFrame("G4/run/run_","G4/Attack/at_",180);
    TabFr[5]=CreeFrame("G5/run/run_","G5/Attack/at_",180);
    TabFr[6]=CreeFrame("G6/run/run_","G6/Attack/at_",180);
    TabFr[7]=CreeFrame("G7/run/run_","G7/Attack/at_",140);
    TabFr[8]=CreeFrame("G8/run/run_","G8/Attack/at_",140);
    TabFr[9]=CreeFrame("G9/run/run_","G9/Attack/at_",140);
    TabFr[10]=CreeFrame("G10/run/run_","G10/Attack/at_",140);
    TabFr[11]=CreeFrame("G11/run/run_","G11/Attack/at_",140);
    TabFr[12]=CreeFrame("G12/run/run_","G12/Attack/at_",140);


    ///Creation du niveau de vie
    for(i=0;i<6;i++)
    {
      sprintf(filename, "scorevie/life%d.png",i);
      image= gdk_pixbuf_new_from_file(filename, NULL);
      /// Redimensionner l'image de l'objet à la taille souhaitée
      Vie[i]= gdk_pixbuf_scale_simple(image,70, 30, GDK_INTERP_BILINEAR);
    }

}

/*********************************************************************************
Fonction:CreateObj:Fonction qui crée les frames des Objets de l'interface
Entrée:char ch[20]:path de l'objet
        double x:position à l'horizontale de l'objet
        double y:Position à la verticale de l'objet
Sortie:la fonction pour crée l'objet avec ses informations
***********************************************************************************/
Objet *CreateObj(char ch[20],double x,double y)
{
    Objet *O=Talloc(Objet);///ALLOCATION
    TesterAllocation(O);///Tester l'allocation
    O->position=Talloc(Dep);
    TesterAllocation(O->position);

    ///Positionnement
    O->position->X=x;
    O->position->Y=y;

      O->image= gdk_pixbuf_new_from_file(ch, NULL);
    /// Redimensionner l'image de l'objet à la taille souhaitée
     O->scaled= gdk_pixbuf_scale_simple(O->image,150, 200, GDK_INTERP_BILINEAR);

   return(Objet*)O;

}

/*********************************************************************************
Fonction:InitObject:Fonction qui charge les frames des Objets
Entrée:void
Sortie:la fonction charge les paths pour la création des frames
***********************************************************************************/
void InitObject()
{

    ///Chargement des paths
  Tab[0]=CreateObj("Maison.png",120,140);
  Tab[1]=CreateObj("Arbre.png",250,350);
  Tab[2]=CreateObj("Roche.png",700,700);
}

/*********************************************************************************
Fonction:defaultValueGlad:Fonction qui definit des valeurs
                            par defaut pour un gladiateur
Entrée:Interface *I:Interface
Sortie:void
***********************************************************************************/
void defaultValueGlad(Interface *I)
{
   I->G->Agressif=TRUE;///Agressif par defaut
   I->G->nvVie=1000;///Niveau de vie plein
   I->G->nvforce=50;///Force à 50%
   I->G->numArme='C';///Arme par defaut une chaine
   I->G->bouclier=FALSE;///Ne dispose de bouclier
   I->G->nbCgagne=0;///Initialiser son nombre de combat
   I->G->nvExp=0;///Initialiser son experience
   I->G->iswalking=TRUE;///En marche au debut
   I->G->isfighting=FALSE;///Ne combat pas
   I->G->Armure='B';///Poss-de une armure en Bronze
   I->choix=0;///Numero du gladiateur est Zero
   I->G->hidden=FALSE;///le gladiateur n'est pas caché
   I->G->nbhid=0;///Temps pour se cacher initialiser
   I->G->nuGlad=0;///Numero du gladiateur est Zero
   /// Creer un nouveau gladiateur avec des coordonnees et une vitesse aleatoire
   I->G->Pos->X= 810;///Point de sortie
   I->G->Pos->Y = 180;///Point de sortie
   I->G->Vitesse->X = g_random_double_range(1.0, 3.0);
   I->G->Vitesse->Y= g_random_double_range(1.0, 3.0);
   I->G->isControlled= FALSE;///Le gladiateur n'est pas contrôlé
   I->G->current_frame=0;///Initialiser ses frames de deplacement
   I->G->isMovingRight=TRUE;///le gladiateur se deplace vers droite


}
/*********************************************************************************
Fonction:initializeGlad:Fonction pour l'allocation de la memoire pour un gladiateur
Entrée:Interface *interface:Interface
Sortie:la fonction retourne l'interface avec un gladiateur et des valeurs par defaut
***********************************************************************************/
Interface *initializeGlad(Interface *interface)
{

    ///ALLOCATION ET TEST DES ALLOCATIONS
    interface->G=Talloc(Gladiateur);///la structure du gladiateur
    TesterAllocation(interface->G);

    interface->G->Pos=Talloc(Dep);///Les deplacement
    TesterAllocation(interface->G->Pos);

    interface->G->Vitesse=Talloc(Dep); ///La vitesse
    TesterAllocation(interface->G->Vitesse);

    defaultValueGlad(interface);///Initialisation des valeurs par defaut

     return(Interface*)interface;
}

/*********************************************************************************
Fonction:creeGlad:Fonction pour remplie les informations du gladiateurs avec des
                    valeurs defini
Entrée:Interface *I:L'interface
        int force:Force du gladiateur
        char numAr:Type de l'arme
        gboolean Agressif:Agressivité du gladieteur
        char type_Armure:Type de l'armure
Sortie:void
***********************************************************************************/
void creeGlad(Interface *I ,int force,char numAr,gboolean Agressif,char type_Armure)
{
   Gladiateur *G=I->G;
    ///Affection des valeurs predefinis
    G->Armure=type_Armure;///Armure
    G->bouclier=TRUE;///la possession de bouclier
    G->Agressif=Agressif;///Agressiité
    G->nvforce=force;///Force
    G->numArme=numAr;///Typen de l'arme
    G->nbCgagne=0;///Nombre de combat initialisé
    G->isControlled=FALSE;///Le gladiateur n'est pas controlé
    G->nvVie=1000;///Le niveau de vie
    G->hidden=FALSE;///Le gladiateur n'est pas caché
    G->nbhid=0;///Temps pour se cacher renitialiser
    G->nvExp=0; ///Initialisation du niveau d'experience
    G->isfighting=FALSE;///Ne combat pas pour le momment
    G->iswalking=TRUE;///Marche au debut
    G->isMovingRight=TRUE;///Se deplace vers la droite

}
/*********************************************************************************
Fonction:InitGladi:Fonction pour crée des gladiateurs avec des valeurs predefinis
Entrée:Interface *I:L'interface
Sortie:void
***********************************************************************************/
void InitGladi(Interface *I)
{
    ///En fonction du choix de l'utilisateur crée le gladiateur avec ses valeurs
    switch(I->choix)
    {
      case 1:creeGlad(I,80,'S',TRUE,'B');///1er gladiateur
          break;
       case 2:creeGlad(I,50,'M',TRUE,'F');///2e gladiateur
          break;
       case 3: creeGlad(I,70,'C',TRUE,'O');///3e gladiateur
          break;
     case 4:creeGlad(I,50,'D',TRUE,'B');///4e gladiateur
          break;
       case 5:creeGlad(I,50,'H',TRUE,'B');///5e gladiateur
          break;
      case 6:creeGlad(I,60,'S',TRUE,'B');///6e gladiateur
          break;
       case 7:creeGlad(I,80,'C',FALSE,'O');///7e gladiateur
         break;
      case 8:creeGlad(I,60,'C',FALSE,'O');///8e gladiateur
         break;
     case 9:creeGlad(I,80,'H',FALSE,'B');///9e gladiateur
         break;
      case 10:creeGlad(I,50,'M',FALSE,'F');///10e gladiateur
          break;
      case 11:creeGlad(I,70,'H',FALSE,'F');///11e gladiateur
        break;
        default :creeGlad(I,70,'S',FALSE,'F');///12 gladiateur

    }
}
/*********************************************************************************
Fonction:ChoixGlad:Fonction qui recupère les données à chaque clique sur le choix
                    d'un gladiateur
Entrée:GtkWidget *w:l'élément qui emet le signal
        gpointer *data: la donnée passée en paramètre
Sortie:void
***********************************************************************************/
void ChoixGlad(GtkWidget *w,gpointer *data)
{
    Interface *I=(Interface*)data;
    ///Recuperer le choix
    int choix = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(w), "choix"));
    I->choix = choix;
    I->G->nuGlad=choix;
    InitGladi(I);
}

/*********************************************************************************
Fonction:create_gladiator: Fonction pour creer un nouveau gladiateur dans l'arène
Entrée:GtkWidget *widget:l'élément qui emet le signal
        gpointer user_data: la donnée passée en paramètre
Sortie:void
***********************************************************************************/
void create_gladiator(GtkWidget *widget, gpointer user_data)
{
    Interface *I=(Interface*)user_data;


    ///Si la force du gladiateur est inferieur à 40
    if(I->G->nvforce<40)
        I->G->nvforce=40;
    /// Ajouter le nouveau gladiateur à la liste
    gladiators = g_list_append(gladiators, I->G);
    I=initializeGlad(I);

}

/*********************************************************************************
Fonction:detect_collisions:Fonction pour détecter les collisions entre les gladiateurs
Entrée:Interface *interface:L'interface
Sortie:void
***********************************************************************************/
void detect_collisions(Interface *interface)
{
    int i;
     /// Parcourir la liste des gladiateurs
    for (GList *iter1 = gladiators; iter1 != NULL; iter1 = g_list_next(iter1))
    {
        Gladiateur *gladiator1 = (Gladiateur *)iter1->data;
        ///Position avec les objets
        ///Verifier si le gladiateur n'est pas proche des objets
        for(i=0;i<3;i++)
        if( (fabs(gladiator1->Pos->X-Tab[i]->position->X)<50) &&
           (fabs(gladiator1->Pos->Y-Tab[i]->position->Y)<50)
          )
        {   ///Si il est proche des objet et qu'il n'est pas caché, le caché
            if(!gladiator1->hidden)
            {
                gladiator1->hidway=i;///Lieu de cachette
                gladiator1->hidden=TRUE;

            }

            break;
        }

        for (GList *iter2 = gladiators; iter2 != NULL; iter2 = g_list_next(iter2))
        {
            Gladiateur *gladiator2 = (Gladiateur *)iter2->data;
            ///Verifier si les 2 gladiateurs n'occupent pas la même position
            if(  (gladiator1 != gladiator2) &&
                 (fabs(gladiator1->Pos->X - gladiator2->Pos->X) < 40 ) &&
                 (fabs(gladiator1->Pos->Y - gladiator2->Pos->Y) < 40 ) &&
                 (!gladiator1->hidden) && (!gladiator2->hidden)
            )
            ///Verifier si l'un d'eux est Agressif
            if( ( (gladiator1->Agressif) || (gladiator2->Agressif) ) &&
                ( (!gladiator1->isfighting) &&(!gladiator2->isfighting) )
              )
            {
             ///Les preparer pour le combat
                Mise_enCombat(gladiator1);
                Mise_enCombat(gladiator2);
                interface->combat=TRUE;///Confirmer la presence d'un combat
                 gladiator1->Pos->Y=gladiator2->Pos->Y;


           }


       }
    }

}

/*********************************************************************************
Fonction:combat_animation:Fonction pour lancer le combat entre gladiateur
Entrée:Interface *interface:L'interface
Sortie:void
***********************************************************************************/
void combat_animation(Interface *interface)
{

     /// Parcourir la liste des gladiateurs
    for (GList *iter1 = gladiators; iter1 != NULL; iter1 = g_list_next(iter1))
    {
         Gladiateur *gladiator1 = (Gladiateur *)iter1->data;
         for (GList *iter2 = gladiators; iter2 != NULL; iter2 = g_list_next(iter2))
         {
            Gladiateur *gladiator2 = (Gladiateur *)iter2->data;
            ///Verifier si les 2 gladiateurs n'occupent pas la même position
             if( (gladiator1 != gladiator2) &&
                ( fabs(gladiator1->Pos->X - gladiator2->Pos->X) < 40 )&&
                ( fabs(gladiator1->Pos->Y - gladiator2->Pos->Y) < 40) &&
                (!gladiator1->hidden) && (!gladiator2->hidden)
              )
              {
                ///L'un des deux est Agressif declenché le combat
               if(( (gladiator1->Agressif) || (gladiator2->Agressif) ) &&
                 ( (gladiator1->isfighting) && (gladiator2->isfighting) )
                )
               {
                  /// Collision detecter
                  CombatGlad(gladiator1,gladiator2,interface);

                 /// Supprimer les gladiateurs morts de la liste
                  if (gladiator1->nvVie <= 0)
                  {
                    interface->combat=FALSE;
                    gladiators = g_list_remove(gladiators, gladiator1);
                    free(gladiator1); ///libérer la mémoire
                  }
                  if (gladiator2->nvVie <= 0)
                  {
                   interface->combat=FALSE;
                   gladiators = g_list_remove(gladiators, gladiator2);
                   free(gladiator2); /// libérer la mémoire
                  }
               }

            }
       }
    }

}


/*********************************************************************************
Fonction:main_loop:Fonction principale pour l'animation de l'interface
Entrée:Interface *interface:L'interface
Sortie:TRUE
***********************************************************************************/
gboolean main_loop(Interface *interface)
{

 /// Redessiner l'arène
    gtk_widget_queue_draw(interface->arena);

    /// Vérifier les collisions et déclencher les combats si nécessaire
    if(!interface->combat && !interface->is_paused)
      detect_collisions(interface);

    /// Gerer les animations de combat
    if (interface->combat)
       combat_animation(interface);

    /// Mettre a jour les positions des gladiateurs
     update_positions(interface);

    return G_SOURCE_CONTINUE;///TRUE
}

/*********************************************************************************
Fonction:CreeInterface:Fonction pour créer les élement visuels de l'interface
Entrée:Interface *interface:L'interface
Sortie:la fonction retourne l'interface et ses éléments
***********************************************************************************/
Interface *CreeInterface(Interface *interface)
{

     char filename[20];
      GdkPixbuf *pixbuf;
      int row,col ;///Variables pour la representation des elements
     /// Création des boutons avec les images
        GtkWidget *button;
        GtkWidget *image_widget;
    ///Création des Containeurs
     Mongrid *G1;
     GtkWidget *b1=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
     G1=Init_grid();
     gtk_box_set_spacing(interface->Boxe->box,10);
     gtk_box_pack_end(GTK_BOX(interface->Boxe->box), b1, FALSE,TRUE, 0);
     gtk_box_pack_start(GTK_BOX(b1), G1->grid, TRUE,TRUE, 0);

    ///Image de l'interface
    MyImage *Img=init_image("glad.png",90,100);
    creer_image(Img);
    Ajouter_elem_grid(G1,Img->image,0,0);

    ///Création des boutons de l'interface radio
    interface->lab=initLabel(interface->lab,"Choix du Comportement");
    CreerLabel(interface->lab);
    Ajouter_elem_grid(G1,interface->lab->label,0,1);

    interface->B1=init_bouton("Agressif",TRUE,'3');
    creer_button_RadioPere(interface);
    Ajouter_elem_grid(G1,interface->B1->buton,0,2);

     interface->B2=init_bouton("Gentil",FALSE,'3');
    creer_button_radiofils(interface,interface->B1->buton);
    Ajouter_elem_grid(G1,interface->B2->buton,1,2);

    ///Zône de choix de la force
    interface->lab=initLabel(interface->lab,"Force");
    CreerLabel(interface->lab);
    Ajouter_elem_grid(G1,interface->lab->label,0,3);

    interface->B1=init_bouton("",TRUE,'4');
    creer_button(interface);
    Ajouter_elem_grid(G1,interface->B1->buton,0,4);

    interface->lab=initLabel(interface->lab,"Choix du Gladiateur");
    CreerLabel(interface->lab);
    Ajouter_elem_grid(G1,interface->lab->label,0,5);



    /// Création d'un grid pour les gladiateurs
    Mongrid *grid=Init_grid();
     Ajouter_elem_grid(G1,grid->grid,0,6);


    /// Chargement des images pour les boutons
    for (int i = 1; i <= 12; i++)
    {
        sprintf(filename, "IconsCaracters/glad%d.png", i);
        pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
       image_widget = gtk_image_new_from_pixbuf(pixbuf);
        button = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(button), image_widget);
        g_object_set_data(G_OBJECT(button), "choix", GINT_TO_POINTER(i));
        g_signal_connect(button, "clicked", G_CALLBACK(ChoixGlad), interface);

        row = (i - 1) / 3;
        col = (i - 1) % 3;
        Ajouter_elem_grid(grid,button,col,row);

    }

    ///bouton pour création des gladiateurs
    Mongrid *G2=Init_grid();
    gtk_box_pack_end(GTK_BOX(b1), G2->grid, TRUE,FALSE, 10);
    interface->B1=init_bouton("Creer Gladiateur",FALSE,'1');
    creer_button(interface);
    Ajouter_elem_grid(G2,interface->B1->buton,0,0);
    g_signal_connect(interface->B1->buton, "clicked", G_CALLBACK(create_gladiator),interface);

    return interface;
}

/*********************************************************************************
Fonction:CreeInterface:Fonction d'initialisation de l'interface utilisateur
Entrée:Interface *interface:L'interface
Sortie:void
***********************************************************************************/
void *initialize_interface(Interface *interface)
{
    /// Creer une fenetre principale
    interface=Talloc(Interface);
    TesterAllocation(interface);

    ///Cree la fénètre de l'interface
     interface->F=init_fenetre("Jeu Gladiateur","img.png",1900,1600);
     interface->ARENA_WIDTH=1400;///Largeur de l'arène
     interface->ARENA_HEIGHT=920;///Hauteuur de l'arène
     strcpy(interface->Arenabac,"bac.png");///Background de l'arène
     interface->is_paused=FALSE;///L'interface n'est pas en pause
     interface->combat=FALSE;///il n'ya pas de combat dans l'interface
     creer_fenetre(interface,TRUE);///Création de la fenètre
     interface=initializeGlad(interface);

    /// Crer une boite pour les widgets
    interface->Boxe=init_box(interface->Boxe,'0');
    creer_Box(interface->Boxe);
    ///Ajouter la box a la fenetre
    gtk_container_add(GTK_CONTAINER(interface->F->window),interface->Boxe->box);

    ///Création de la Zône de dessin
     interface->arena= gtk_drawing_area_new();
      gtk_box_pack_start(GTK_BOX(interface->Boxe->box), interface->arena, TRUE, TRUE, 0);

    ///Dessiner l'interface
    g_signal_connect(interface->arena, "draw", G_CALLBACK(on_draw), interface);

    /// Connecter les signaux pour les évènements de la souris
    gtk_widget_add_events(interface->arena, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    g_signal_connect(interface->arena, "motion-notify-event", G_CALLBACK(on_mouse_motion), interface);
    g_signal_connect(interface->arena, "button-press-event", G_CALLBACK(on_mouse_click), interface);

  /// Demarrer le timer pour mettre a jour les positions des gladiateurs a des intervalles reguliers
    g_timeout_add(90, (GSourceFunc)main_loop,interface);

    interface=CreeInterface(interface);

}

/*********************************************************************************
Fonction:create_game_window:Fonction pour la création de la fénètre de jeu
Entrée:void
Sortie:void
***********************************************************************************/
void create_game_window()
{
    ///Initialiser l'interface
    Interface *interface;
    interface=initialize_interface(interface);

    ///Afficher touts les widgets
   gtk_widget_show_all(interface->F->window);
    gtk_main();
}

/*********************************************************************************
Fonction:on_play_button_clicked:Fonction qui reçoit le signal pour debuter le jeu
Entrée :GtkWidget *widget:l'élément qui emet le signal
        gpointer user_data: la donnée passée en paramètre
Sortie:void
***********************************************************************************/
void on_play_button_clicked(GtkWidget *widget, gpointer user_data)
{
    Interface *I=(Interface*)user_data;
    gtk_widget_destroy(I->F->window);
    free(I);
    create_game_window();
}

/*********************************************************************************
Fonction:on_quitter_button_clicked:Fonction qui reçoit le signal pour quitter le jeu
Entrée :GtkWidget *widget:l'élément qui emet le signal
        gpointer user_data: la donnée passée en paramètre
Sortie:void
***********************************************************************************/
void on_quitter_button_clicked(GtkWidget *widget, gpointer user_data)
{
    gtk_main_quit();
}

/*********************************************************************************
Fonction:mainfenetre:Fonction qui crée la fenètre d'accueil
Entrée :int argc: Nombre d'argument en mode console
        char *argv[]:chaine de caractère de ses arguments
Sortie:void
***********************************************************************************/
void mainfenetre(int argc, char *argv[])
{
    gtk_init(&argc, &argv);///Initialisation de l'interface
    ///Variables et allocation
    GtkWidget *play_button, *quitter_button;
    GtkWidget *play_image, *quitter_image;
    Interface *I=Talloc(Interface);
    TesterAllocation(I);

    ///Créer des boxes pour mes elements
    GtkWidget *box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
    gtk_box_pack_end(box,box1,FALSE,FALSE,0);
    /// Créer la fenêtre principale
    I->F=init_fenetre("Accueil Game","Aceuil.jpg",1000,700);
    creer_fenetre(I,FALSE);

    ///chargement et création de l'image de l'accueil
    I->Image=Talloc(MyImage);
    TesterAllocation(I->Image);
    I->Image=init_image("Aceuil.jpg",1000,600);
    creer_image(I->Image);
    gtk_box_pack_start(box,I->Image->image,FALSE,TRUE,0);

  /// Créer et ajouter le bouton Play avec une image
    play_button = gtk_button_new();
    play_image = gtk_image_new_from_file("bt.png");
    gtk_button_set_image(GTK_BUTTON(play_button), play_image);
     gtk_box_pack_start(box1,play_button,TRUE,FALSE,0);
    g_signal_connect(play_button, "clicked", G_CALLBACK(on_play_button_clicked),I);


    /// Créer et ajouter le bouton Quitter avec une image
    quitter_button = gtk_button_new();
    quitter_image = gtk_image_new_from_file("boutonQuitter.png");
    gtk_button_set_image(GTK_BUTTON(quitter_button), quitter_image);
    gtk_box_pack_end(box1,quitter_button,TRUE,FALSE,0);
    g_signal_connect(quitter_button, "clicked", G_CALLBACK(on_quitter_button_clicked), NULL);


    gtk_container_add(GTK_CONTAINER(I->F->window), box);


    /// Afficher tous les widgets
    gtk_widget_show_all(I->F->window);
    gtk_main();

}

/*********************************************************************************
Fonction:main:Fonction principal
Entrée :int argc: Nombre d'argument en mode console
        char *argv[]:chaine de caractère de ses arguments
Sortie:0
***********************************************************************************/
int main(int argc,char **argv)
{

     InitObject();///Initialiser les objets de l'interface
    initFrameGlad();///Cree les frames de l'interface
    mainfenetre(argc,argv);
    return 0;
}
