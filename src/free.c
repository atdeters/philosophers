/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 12:26:11 by andreas           #+#    #+#             */
/*   Updated: 2025/04/29 01:31:39 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void	free_philos(t_philo **lst, int nbp)
{
	int	i;

	i = 0;
	while (lst && lst[i] && i < nbp)
	{
		free(lst[i]);
		i++;
	}
	free(lst);
	lst = NULL;
}

void	destroy_mutex_nb(pthread_mutex_t *mutexes, int nb)
{
	int	i;

	i = 0;
	while (i < nb)
	{
		pthread_mutex_destroy(&mutexes[i]);
		i++;
	}
}

void	destroy_threads_nb(pthread_t *threads, int nb)
{
	int	i;

	i = 0;
	while (i < nb)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

void	free_allos(t_philo ***philos, pthread_t **threads,
	pthread_mutex_t **mutexes, int nbp)
{
	if (mutexes && *mutexes)
	{
		free(*mutexes);
		*mutexes = NULL;
	}
	if (threads && *threads)
	{
		free(*threads);
		*threads = NULL;
	}
	if (philos && *philos)
	{
		free_philos(*philos, nbp);
		*philos = NULL;
	}
}
