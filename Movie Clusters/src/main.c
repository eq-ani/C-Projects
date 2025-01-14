#include <assert.h>
#include <recommendation_system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char* movie_names[12] = {"GameNight",
                           "PineappleExpress",
                           "Borat",
                           "TheShining",
                           "Split",
                           "It",
                           "Matilda",
                           "HarryPotter",
                           "TheBatman",
                           "DonnieBrasco",
                           "AmericanGangster",
                           "TheGodfather"};
  Cluster clusters[4] = {
      {0, {0, 0}, NULL},  // First element
      {1, {0, 0}, NULL},  // Second element
      {2, {0, 0}, NULL},  // Third element
      {3, {0, 0}, NULL}   // Fourth element
  };
  ;
  Movie movies[12];

  // GameNight
  movies[0].movie_name = (char*)malloc(strlen(movie_names[0]) + 1);
  strcpy(movies[0].movie_name, movie_names[0]);
  movies[0].movie_parameters.a = 1;
  movies[0].movie_parameters.b = 2;

  // PineappleExpress
  movies[1].movie_name = (char*)malloc(strlen(movie_names[1]) + 1);
  strcpy(movies[1].movie_name, movie_names[1]);
  movies[1].movie_parameters.a = 2;
  movies[1].movie_parameters.b = 2;

  // Borat
  movies[2].movie_name = (char*)malloc(strlen(movie_names[2]) + 1);
  strcpy(movies[2].movie_name, movie_names[2]);
  movies[2].movie_parameters.a = 3;
  movies[2].movie_parameters.b = 3;

  // TheShining
  movies[3].movie_name = (char*)malloc(strlen(movie_names[3]) + 1);
  strcpy(movies[3].movie_name, movie_names[3]);
  movies[3].movie_parameters.a = 10;
  movies[3].movie_parameters.b = 10;

  // Split
  movies[4].movie_name = (char*)malloc(strlen(movie_names[4]) + 1);
  strcpy(movies[4].movie_name, movie_names[4]);
  movies[4].movie_parameters.a = 11;
  movies[4].movie_parameters.b = 11;

  // It
  movies[5].movie_name = (char*)malloc(strlen(movie_names[5]) + 1);
  strcpy(movies[5].movie_name, movie_names[5]);
  movies[5].movie_parameters.a = 12;
  movies[5].movie_parameters.b = 12;

  // Matilda
  movies[6].movie_name = (char*)malloc(strlen(movie_names[6]) + 1);
  strcpy(movies[6].movie_name, movie_names[6]);
  movies[6].movie_parameters.a = 140;
  movies[6].movie_parameters.b = 140;

  // HarryPotter
  movies[7].movie_name = (char*)malloc(strlen(movie_names[7]) + 1);
  strcpy(movies[7].movie_name, movie_names[7]);
  movies[7].movie_parameters.a = 141;
  movies[7].movie_parameters.b = 141;

  // Batman
  movies[8].movie_name = (char*)malloc(strlen(movie_names[8]) + 1);
  strcpy(movies[8].movie_name, movie_names[8]);
  movies[8].movie_parameters.a = 142;
  movies[8].movie_parameters.b = 142;

  // DonnieBrasco
  movies[9].movie_name = (char*)malloc(strlen(movie_names[9]) + 1);
  strcpy(movies[9].movie_name, movie_names[9]);
  movies[9].movie_parameters.a = 80;
  movies[9].movie_parameters.b = 80;

  // AmericanGangster
  movies[10].movie_name = (char*)malloc(strlen(movie_names[10]) + 1);
  strcpy(movies[10].movie_name, movie_names[10]);
  movies[10].movie_parameters.a = 81;
  movies[10].movie_parameters.b = 81;

  // Godfather
  movies[11].movie_name = (char*)malloc(strlen(movie_names[11]) + 1);
  strcpy(movies[11].movie_name, movie_names[11]);
  movies[11].movie_parameters.a = 82;
  movies[11].movie_parameters.b = 82;

  for (int i = 0; i < 12; i++) {
    movies[i].cluster_id = -1;
  }

  int result;
  printf("\n=======  Test add_movies  ========\n\n");
  movies[0].cluster_id = 1;
  result = add_movies(clusters, 4, &movies[0], 1);
  printf("The result should be 1 and your result is %i\n", result);
  printf(
      "The first movie in Cluster 1 should be GameNight. In your Cluster 1, "
      "it is %s.\n",
      (clusters[1].list)->movie->movie_name);

  Movie* selected_movies[4];
  for (int i = 0; i < 4; ++i) {
    selected_movies[i] = &movies[i + 1];
    movies[i + 1].cluster_id = i;
  }
  result = add_movies(clusters, 4, *selected_movies, 4);
  printf("The result should be 4 and your result is %i\n", result);
  printf(
      "The first movie in Cluster 0 should be PineappleExpress. In your "
      "Cluster 0, it is %s.\n",
      (clusters[0].list)->movie->movie_name);
  printf(
      "The first movie in Cluster 1 should be Borat. In your Cluster 1, it is "
      "%s.\n",
      (clusters[1].list)->movie->movie_name);
  printf(
      "The second movie in Cluster 1 should be GameNight. In your Cluster 1, "
      "it is %s.\n",
      (clusters[1].list)->next->movie->movie_name);
  printf(
      "The first movie in Cluster 2 should be TheShining. In your Cluster 2, "
      "it is %s.\n",
      (clusters[2].list)->movie->movie_name);
  printf(
      "The first movie in Cluster 3 should be Split. In your Cluster 3, it is "
      "%s.\n",
      (clusters[3].list)->movie->movie_name);

  printf("\n=======  Test remove_movie  ========\n\n");
  Movie* removed_movie = remove_movie(clusters, 4, &movies[0]);
  printf("You should have removed GameNight. You removed %s.\n",
         (removed_movie->movie_name));
  assert(clusters[1].list != NULL);
  printf("Cluster 1 should have Borat. Your cluster 1 has %s.\n",
         (clusters[1].list->movie->movie_name));

  removed_movie = remove_movie(clusters, 4, &movies[1]);
  printf("You should have removed PineappleExpress. You removed %s.\n",
         (removed_movie->movie_name));
  assert(clusters[0].list == NULL);

  printf("\n======= Test Multiple Add and Remove =======\n");
  movies[6].cluster_id = 1;
  movies[7].cluster_id = 1;
  movies[8].cluster_id = 1;

  printf("\nAdding movies\n");
  result = add_movies(clusters, 4, &movies[6], 1);  // adding to the end
  result = add_movies(clusters, 4, &movies[7], 1);  // adding in the middle
  result = add_movies(clusters, 4, &movies[8], 1);  // adding to the end
  printf("First movie in Cluster 1 should be Borat. Yours is %s.\n",
         (clusters[1].list->movie->movie_name));
  printf("Second movie in Cluster 1 should be HarryPotter. Yours is %s.\n",
         (clusters[1].list->next->movie->movie_name));
  printf("Third movie in Cluster 1 should be Matilda. Yours is %s.\n",
         (clusters[1].list->next->next->movie->movie_name));
  printf("Fourth movie in Cluster 1 should be TheBatman. Yours is %s.\n",
         (clusters[1].list->next->next->next->movie->movie_name));

  printf("\nRemoving First Movie\n");
  removed_movie = remove_movie(clusters, 4, &movies[2]);
  printf("Should have removed Borat. You removed %s\n",
         removed_movie->movie_name);
  printf("First movie in Cluster 1 should be HarryPotter. Yours is %s.\n",
         (clusters[1].list->movie->movie_name));
  printf("Second movie in Cluster 1 should be Matilda. Yours is %s.\n",
         (clusters[1].list->next->movie->movie_name));
  printf("Third movie in Cluster 1 should be TheBatman. Yours is %s.\n",
         (clusters[1].list->next->next->movie->movie_name));

  printf("\nRemoving Middle Movie\n");
  removed_movie = remove_movie(clusters, 4, &movies[6]);
  printf("Should have removed Matilda. You removed %s\n",
         removed_movie->movie_name);
  printf("First movie in Cluster 1 should be HarryPotter. Yours is %s.\n",
         (clusters[1].list->movie->movie_name));
  printf("Second movie in Cluster 1 should be TheBatman. Yours is %s.\n",
         (clusters[1].list->next->movie->movie_name));

  printf("\nRemoving Last Movie\n");
  removed_movie = remove_movie(clusters, 4, &movies[8]);
  printf("Should have removed TheBatman. You removed %s\n",
         removed_movie->movie_name);
  printf("First movie in Cluster 1 should be HarryPotter. Yours is %s.\n",
         (clusters[1].list->movie->movie_name));

  removed_movie = remove_movie(clusters, 4, &movies[3]);
  printf("Should have removed TheShining. You removed %s\n",
         removed_movie->movie_name);
  assert(clusters[2].list == NULL);
  // exit(1);

  printf("\n======= Test move_movie_to_cluster =======\n");

  printf("\nAdding Movies\n");
  clusters[0].list = NULL;
  clusters[2].list = NULL;
  clusters[3].list = NULL;
  result = add_movies(clusters, 4, &movies[6], 1);
  result = add_movies(clusters, 4, &movies[2], 1);
  result = add_movies(clusters, 4, &movies[8], 1);
  printf("First movie in Cluster 1 should be Borat. Yours is %s.\n",
         (clusters[1].list->movie->movie_name));
  printf("Second movie in Cluster 1 should be HarryPotter. Yours is %s.\n",
         (clusters[1].list->next->movie->movie_name));
  printf("Third movie in Cluster 1 should be Matilda. Yours is %s.\n",
         (clusters[1].list->next->next->movie->movie_name));
  printf("Fourth movie in Cluster 1 should be TheBatman. Yours is %s.\n",
         (clusters[1].list->next->next->next->movie->movie_name));

  printf("\nMoving Movies\n");
  int moved = move_movie_to_cluster(1, clusters, 4, &movies[6]);
  printf("Moving Movie %s from Cluster 1 to Cluster 1\n", movies[6].movie_name);
  printf(
      "Moved must be 0 because movie already in that cluster. You returned "
      "%d.\n",
      moved);
  printf("Third movie in Cluster 1 should be Matilda. Yours is %s.\n",
         (clusters[1].list->next->next->movie->movie_name));
  assert(clusters[0].list == NULL);
  //   assert(cluster[1].list==NULL);
  assert(clusters[2].list == NULL);
  assert(clusters[3].list == NULL);

  moved = move_movie_to_cluster(0, clusters, 4, &movies[6]);
  printf("\nMoving Movie %s from Cluster 1 to Cluster 0\n",
         movies[6].movie_name);
  printf("Moved must be 1. You returned %d.\n", moved);
  printf("Third movie in Cluster 1 should be TheBatman. Yours is %s.\n",
         (clusters[1].list->next->next->movie->movie_name));
  printf("First movie in Cluster 0 should be Matilda. Yours is %s.\n",
         (clusters[0].list->movie->movie_name));
  //   assert(cluster[1].list==NULL);
  assert(clusters[2].list == NULL);
  assert(clusters[3].list == NULL);

  move_movie_to_cluster(0, clusters, 4, &movies[2]);
  move_movie_to_cluster(0, clusters, 4, &movies[8]);
  moved = move_movie_to_cluster(0, clusters, 4, &movies[7]);
  printf("\nMoving Movies %s and %s from Cluster 1 to Cluster 0\n",
         movies[2].movie_name, movies[8].movie_name);
  printf("Moved must be 1. You returned %d.\n", moved);
  printf("First movie in Cluster 0 should be Borat. Yours is %s.\n",
         (clusters[0].list->movie->movie_name));
  printf("Second movie in Cluster 0 should be HarryPotter. Yours is %s.\n",
         (clusters[0].list->next->movie->movie_name));
  printf("Third movie in Cluster 0 should be Matilda. Yours is %s.\n",
         (clusters[0].list->next->next->movie->movie_name));
  printf("Fourth movie in Cluster 0 should be TheBatman. Yours is %s.\n\n\n",
         (clusters[0].list->next->next->next->movie->movie_name));

  printf("\n======= Clustering Details =======\n");
  printf("Emptying Clusters First\n");
  clusters[0].list = NULL;
  clusters[1].list = NULL;
  clusters[2].list = NULL;
  clusters[3].list = NULL;
  Cluster* arranged_clusters = k_means_clustering(clusters, 4, movies, 12);

  return 0;
}
