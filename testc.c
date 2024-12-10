#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Livre {
    int id;
    char titre[100];
    char auteur[100];
    int annee;
    int emprunte; 
    struct Livre* suivant;
} Livre;

typedef struct Pile {
    int idLivre;      
    int action;       
    struct Pile* suivant; 
} Pile;

Livre* ajouterLivre(Livre* tete, int id, const char* titre, const char* auteur, int annee) {
    Livre* nouveau = (Livre*)malloc(sizeof(Livre));
    nouveau->id = id;
    strcpy(nouveau->titre, titre);
    strcpy(nouveau->auteur, auteur);
    nouveau->annee = annee;
    nouveau->emprunte = 0;  
    nouveau->suivant = tete;
    return nouveau;
}

void emprunterLivre(Livre* tete, Pile** sommet, int id) {
    Livre* courant = tete;
    while (courant != NULL) {
        if (courant->id == id) {
            if (courant->emprunte == 0) {
                courant->emprunte = 1;  
                printf("Le livre '%s' a été emprunté.\n", courant->titre);
                Pile* nouvelleAction = (Pile*)malloc(sizeof(Pile));
                nouvelleAction->idLivre = id;
                nouvelleAction->action = 1;  
                nouvelleAction->suivant = *sommet;
                *sommet = nouvelleAction;
                return;
            } else {
                printf("Le livre '%s' est déjà emprunté.\n", courant->titre);
                return;
            }
        }
        courant = courant->suivant;
    }
    printf("Livre non trouvé.\n");
}

void retournerLivre(Livre* tete, Pile** sommet, int id) {
    Livre* courant = tete;
    while (courant != NULL) {
        if (courant->id == id) {
            if (courant->emprunte == 1) {
                courant->emprunte = 0;  
                printf("Le livre '%s' a été retourné.\n", courant->titre);
                Pile* nouvelleAction = (Pile*)malloc(sizeof(Pile));
                nouvelleAction->idLivre = id;
                nouvelleAction->action = 0;  
                nouvelleAction->suivant = *sommet;
                *sommet = nouvelleAction;
                return;
            } else {
                printf("Le livre '%s' n'a pas été emprunté.\n", courant->titre);
                return;
            }
        }
        courant = courant->suivant;
    }
    printf("Livre non trouvé.\n");
}

void annulerDerniereAction(Livre* tete, Pile** sommet) {
    if (*sommet == NULL) {
        printf("Aucune action à annuler.\n");
        return;
    }
    Pile* action = *sommet;
    *sommet = action->suivant; 
    int id = action->idLivre;
    int actionType = action->action;

    Livre* courant = tete;
    while (courant != NULL) {
        if (courant->id == id) {
            if (actionType == 1) {
                courant->emprunte = 0;  
                printf("Annulation de l'emprunt du livre '%s'.\n", courant->titre);
            } else {
                courant->emprunte = 1;
                printf("Annulation du retour du livre '%s'.\n", courant->titre);
            }
            free(action);  
            return;
        }
        courant = courant->suivant;
    }
}

void supprimerLivre(Livre** tete, int id) {
    if (*tete == NULL) {
        printf("Aucun livre à supprimer.\n");
        return;
    }
    Livre* courant = *tete;
    Livre* precedent = NULL;

    while (courant != NULL && courant->id != id) {
        precedent = courant;
        courant = courant->suivant;
    }
    if (courant == NULL) {
        printf("Livre avec ID %d non trouvé.\n", id);
        return;
    }
    if (precedent == NULL) {
        *tete = courant->suivant;
    } else { 
        precedent->suivant = courant->suivant;
    }

    printf("Le livre '%s' a été supprimé.\n", courant->titre);
    free(courant); 
}

void afficherLivres(Livre* tete) {
    if (tete == NULL) {
        printf("Aucun livre dans la bibliothèque.\n");
        return;
    }

    printf("\n--- Liste des livres ---\n");
    Livre* courant = tete;
    while (courant != NULL) {
        printf("ID: %d, Titre: %s, Auteur: %s, Année: %d, %s\n", courant->id, courant->titre, courant->auteur, courant->annee,
               courant->emprunte ? "Emprunté" : "Disponible");
        courant = courant->suivant;
    }
}

void sauvegarderLivres(Livre* tete, const char* nomFichier) {
    FILE* fichier = fopen(nomFichier, "w");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    Livre* courant = tete;
    while (courant != NULL) {
        fprintf(fichier, "%d|%s|%s|%d|%d\n", courant->id, courant->titre, courant->auteur, courant->annee, courant->emprunte);
        courant = courant->suivant;
    }

    fclose(fichier);
    printf("Données sauvegardées dans le fichier.\n");
}

Livre* chargerLivres(const char* nomFichier) {
    FILE* fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Aucun fichier trouvé, la bibliothèque est vide.\n");
        return NULL;
    }

    Livre* tete = NULL;
    int id, annee, emprunte;
    char titre[100], auteur[100];

    while (fscanf(fichier, "%d|%99[^|]|%99[^|]|%d|%d\n", &id, titre, auteur, &annee, &emprunte) == 5) {
        tete = ajouterLivre(tete, id, titre, auteur, annee);
        Livre* courant = tete;
        while (courant != NULL && courant->id != id) {
            courant = courant->suivant;
        }
        if (courant) {
            courant->emprunte = emprunte;
        }
    }

    fclose(fichier);
    return tete;
}

void libererLivres(Livre* tete) {
    while (tete != NULL) {
        Livre* tmp = tete;
        tete = tete->suivant;
        free(tmp);
    }
}

void libererPile(Pile* sommet) {
    while (sommet != NULL) {
        Pile* tmp = sommet;
        sommet = sommet->suivant;
        free(tmp);
    }
}

int main() {
    Livre* tete = NULL; 
    Pile* sommet = NULL;
    char nomFichier[] = "bibliotheque.txt";
    tete = chargerLivres(nomFichier);

    int choix, id, annee;
    char titre[100], auteur[100];

    do {
        printf("\n--- Gestion de Bibliothèque ---\n");
        printf("1. Ajouter un livre\n");
        printf("2. Emprunter un livre\n");
        printf("3. Retourner un livre\n");
        printf("4. Annuler la dernière action\n");
        printf("5. Afficher les livres\n");
        printf("6. Supprimer un livre\n");
        printf("7. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("ID : ");
                scanf("%d", &id);
                getchar();
                printf("Titre : ");
                fgets(titre, sizeof(titre), stdin);
                titre[strcspn(titre, "\n")] = '\0';
                printf("Auteur : ");
                fgets(auteur, sizeof(auteur), stdin);
                auteur[strcspn(auteur, "\n")] = '\0';
                printf("Année : ");
                scanf("%d", &annee);
                tete = ajouterLivre(tete, id, titre, auteur, annee);
                break;

            case 2:
                printf("ID du livre à emprunter : ");
                scanf("%d", &id);
                emprunterLivre(tete, &sommet, id);
                break;

            case 3:
                printf("ID du livre à retourner : ");
                scanf("%d", &id);
                retournerLivre(tete, &sommet, id);
                break;

            case 4:
                annulerDerniereAction(tete, &sommet);
                break;

            case 5:
                afficherLivres(tete);
                break;

            case 6:
                printf("ID du livre à supprimer : ");
                scanf("%d", &id);
                supprimerLivre(&tete, id);
                break;

            case 7:
                sauvegarderLivres(tete, nomFichier);
                libererLivres(tete);
                libererPile(sommet);
                printf("Données sauvegardées et mémoire libérée.\n");
                break;

            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 7);

    return 0;
}