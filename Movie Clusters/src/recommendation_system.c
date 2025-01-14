#include <limits.h>
#include <math.h>
#include <recommendation_system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int calculate_distance(Parameters cluster_parameters, Parameters movie_parameters) {
  return pow(pow(cluster_parameters.a - movie_parameters.a, 2) +
                 pow(cluster_parameters.b - movie_parameters.b, 2),
             0.5);
}

int add_movies(Cluster cluster[], int number_of_clusters, Movie movies[], int number_of_movies) 
{
  int total_added = 0;
  for (int i = 0; i < number_of_movies; i++)
  {
    Movie* mov = &movies[i];
    //find cluster we want to be in
    Cluster* clus = NULL;
    for (int j = 0; j < number_of_clusters; j++)
    {
      if (cluster[j].cluster_id == mov->cluster_id)
      {
        clus = &cluster[j];
        break;
      }
    }
    if (clus == NULL)
    {
      continue;
    }
    //if theres nothing in the cluster
    if (clus->list == NULL)
    {
      Cluster_Movies* clus_movies = (Cluster_Movies*)malloc(sizeof(Cluster_Movies));
      clus_movies->movie = mov;
      clus_movies->next = NULL;
      clus->list = clus_movies;
      total_added++;
      continue;
    }
    //otherwise we need to find where it should be within the "list"
    Cluster_Movies* curr = clus->list;
    Cluster_Movies* prev = NULL;
    int c = strcmp(curr->movie->movie_name, mov->movie_name); //compares whatever is at the head of the movie list to the movie
    while (c < 0 && curr != NULL)
    {
      prev = curr;
      curr = curr->next; //loops until its at the right spot
      if (curr != NULL)
      {
        c = strcmp(curr->movie->movie_name, mov->movie_name);
      }
    }
    if (c == 0)
    {
      continue; //since its the same just skip it and head back to the top
    }
    //we add to the cluster
    Cluster_Movies* clus_movies = (Cluster_Movies*)malloc(sizeof(Cluster_Movies));
    clus_movies->movie = mov;
    clus_movies->next = curr;
    if (prev != NULL)
    {
      prev->next = clus_movies;
    }
    else
    {
      clus->list = clus_movies;
    }
    total_added++;
  }
  return total_added;
}

Movie* remove_movie(Cluster cluster[], int number_of_clusters, Movie* movie) 
{
  Cluster* clus = NULL; //same thing as before, find the right cluster
  for (int j = 0; j < number_of_clusters; j++)
  {
    if (cluster[j].cluster_id == movie->cluster_id)
    {
      clus = &cluster[j];
      break;
    }
  }
  //if the cluster doesn't exist or the movie cluster doesn't exist 
  //the deletion can't go through, ie return null
  if (clus == NULL || clus->list == NULL)
  {
    return NULL;
  }
  //now we want to navigate to the correct place in the movie cluster
  Cluster_Movies* curr = clus->list;
  Cluster_Movies* prev = NULL;
  int c = strcmp(curr->movie->movie_name, movie->movie_name); //compares whatever is at the head of the movie list to the movie
  while (c < 0 && curr != NULL)
  {
    prev = curr;
    curr = curr->next; //loops until its at the right spot
    if (curr != NULL)
    {
      c = strcmp(curr->movie->movie_name, movie->movie_name);
    }
  }
  if (c == 0 && curr != NULL)
  {
    if (prev == NULL) //make the head our "curr.next"
    {
      clus->list = curr->next;
    }
    else //in any other case make prev skip over curr
    {
      prev->next = curr->next;
    }
    return curr->movie;
  }
  return NULL;
}

int move_movie_to_cluster(int cluster_id, Cluster cluster[], int number_of_clusters, Movie* movie) 
{
  //this is a copy of the movie that we are going to be deleting with the movies old id
  Movie* mov = (Movie*)malloc(sizeof(Movie));
  mov->cluster_id = movie->cluster_id;
  mov->movie_name = movie->movie_name;
  mov->movie_parameters = movie->movie_parameters;
  //this is the movie that we are going to be moving
  movie->cluster_id = cluster_id;
  //now add the the movie to the cluster
  int addcheck = add_movies(cluster, number_of_clusters, movie, 1);
  //then remove the original movie from the cluster
  if (addcheck == 0)
  {
    return 0;
  }
  remove_movie(cluster, number_of_clusters, mov);
  
  return 1;
  
}

// NO TODOs below. Supplemental application: You do not need to look at this. We
// will use your functions above to implement an algorithm to cluster/group
// similar movies together.
Cluster* k_means_clustering(Cluster cluster[], int number_of_clusters,
                            Movie movies[], int number_of_movies) {
  // // Movies are not in any clusters. So their Cluster_ids are -1.
  for (int i = 0; i < number_of_movies; i++) {
    movies[i].cluster_id = -1;
  }

  // Initialize centroids using K-means++ method
  // Choose the first centroid randomly
  int first_centroid_idx = rand() % number_of_movies;
  movies[first_centroid_idx].cluster_id = 0;
  add_movies(cluster, number_of_clusters, &movies[first_centroid_idx], 1);
  cluster[0].cluster_parameters.a =
      movies[first_centroid_idx].movie_parameters.a;
  cluster[0].cluster_parameters.b =
      movies[first_centroid_idx].movie_parameters.b;

  // Choose remaining centroids probabilistically
  for (int i = 1; i < number_of_clusters; i++) {
    double sum_of_distances = 0;
    for (int j = 0; j < number_of_movies; j++) {
      int closest_distance = INT_MAX;
      for (int k = 0; k < i; k++) {
        int distance = calculate_distance(cluster[k].cluster_parameters,
                                          movies[j].movie_parameters);
        if (distance < closest_distance) {
          closest_distance = distance;
        }
      }
      sum_of_distances += closest_distance * closest_distance;
    }

    double target = ((double)rand() / (RAND_MAX)) * sum_of_distances;
    double cumulative_distance = 0;
    int chosen_index = -1;
    for (int j = 0; j < number_of_movies; j++) {
      int closest_distance = INT_MAX;
      for (int k = 0; k < i; k++) {
        int distance = calculate_distance(cluster[k].cluster_parameters,
                                          movies[j].movie_parameters);
        if (distance < closest_distance) {
          closest_distance = distance;
        }
      }
      cumulative_distance += closest_distance * closest_distance;
      if (cumulative_distance >= target) {
        chosen_index = j;
        break;  // Exit the loop as we've found our next centroid
      }
    }

    movies[chosen_index].cluster_id = i;
    add_movies(cluster, number_of_clusters, &movies[chosen_index], 1);
    cluster[i].cluster_parameters.a = movies[chosen_index].movie_parameters.a;
    cluster[i].cluster_parameters.b = movies[chosen_index].movie_parameters.b;
  }

  printf("\n\nIteration: 0");
  for (int i = 0; i < number_of_clusters; i++) {
    printf("\nIn Cluster %d: ", i);
    Cluster_Movies* curr = cluster[i].list;
    while (curr != NULL) {
      printf("%s, ", curr->movie->movie_name);
      curr = curr->next;
    }
  }

  int num_moved = 1;
  int iteration = 0;
  while (num_moved != 0) {
    iteration += 1;

    // Assign movies to clusters
    num_moved = 0;
    for (int j = 0; j < number_of_movies; j++) {
      int closest_cluster = 0;
      int closest_cluster_distance = INT_MAX;

      // Find the cluster whose cluster_parameters are closest to this movie's
      // movie_parameters
      for (int k = 0; k < number_of_clusters; k++) {
        int distance = calculate_distance(cluster[k].cluster_parameters,
                                          movies[j].movie_parameters);
        if (distance < closest_cluster_distance) {
          closest_cluster_distance = distance;
          closest_cluster = k;
        }
      }

      // If num_moved is zero, clustering has converged.
      // Move movies to their closest clusters here and record if they were
      // moved.
      num_moved += move_movie_to_cluster(closest_cluster, cluster,
                                         number_of_clusters, &movies[j]);
    }
    // Update cluster_parameters to
    // the average of the movie_parameters of all movies in the cluster
    for (int k = 0; k < number_of_clusters; k++) {
      int cluster_parameters_a = 0;
      int cluster_parameters_b = 0;
      Cluster_Movies* curr = cluster[k].list;
      int num_movies = 0;
      while (curr != NULL) {
        cluster_parameters_a += curr->movie->movie_parameters.a;
        cluster_parameters_b += curr->movie->movie_parameters.b;
        num_movies += 1;
        curr = curr->next;
      }
      cluster_parameters_a /= num_movies;
      cluster_parameters_b /= num_movies;
      cluster[k].cluster_parameters.a = cluster_parameters_a;
      cluster[k].cluster_parameters.b = cluster_parameters_b;
    }

    printf("\n\nIteration: %d", iteration);
    for (int i = 0; i < number_of_clusters; i++) {
      printf("\nIn Cluster %d: ", i);
      Cluster_Movies* curr = cluster[i].list;
      while (curr != NULL) {
        printf("%s, ", curr->movie->movie_name);
        curr = curr->next;
      }
    }
    printf("\n\n");
  }
}
