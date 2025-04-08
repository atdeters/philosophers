/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/08 12:19:50 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <stdio.h>

//! Needs to stop immideatily when one of the philosopher dies
void	*daily_routine(void *philo)
{
	t_philo *p;

	p = (t_philo *)philo;
	while (!p->data->is_kil)
	{
		//* First grab lower fork number, then the larger one
		//* Aka make the mutex that comes first be dependent on its number
		if (!p_log(p->data, p->philo_nb, FORK))
			break;
		if (!p_log(p->data, p->philo_nb, FORK))
			break;
		if (!p_log(p->data, p->philo_nb, EAT))
			break;
		usleep(p->data->tte);
		p->time_since_meal = time_passed(p->data);
		p->times_eaten++;
		if (p->times_eaten == p->data->nbte)
			break;
		if (!p_log(p->data, p->philo_nb, SLEEP))
			break;
		usleep(p->data->tts);
		if (!p_log(p->data, p->philo_nb, THINK))
			break;
	}
	return (NULL);
}

// Make sure to not fuck up 0-Indexing while 1-Indexing philosophers
t_philo	*constructor(t_data *data, int nb)
{
	t_philo	*p;

	p = malloc(sizeof(t_philo));
	if (!p)
		return (NULL);
	p->data = data;
	p->philo_nb = nb;
	p->time_since_meal = 0;
	p->times_eaten = 0;
	if (nb == 0)
		p->fork_left = p->data->nbp;
	else
		p->fork_left = nb;
	if (nb == p->data->nbp - 1)
		p->fork_right = 1;
	else
		p->fork_right = nb + 1;
	return (p);
}

// TODO: Incorperate the threads and mutexes in the philos array (So everyone has own fork kinda)
int	main(int ac, char **av)
{
	t_data			data;
	t_philo			**philos;
	pthread_t		*threads;
	pthread_mutex_t *mutexes;

	if (init_prog(&data, ac, av))
		return (p_err(data.error));

	// Start timer
	if (gettimeofday(&data.start, NULL) < 0)
		return (ERR_GTOD);

	// Create a list of forks as mutexes
	mutexes = malloc(data.nbp * sizeof(pthread_mutex_t));

	mutexes[0] = (pthread_mutex_t){0};
	mutexes[1] = (pthread_mutex_t){0};
	pthread_mutex_init(&mutexes[0], NULL);
	pthread_mutex_init(&mutexes[1], NULL);

	// Create a list of philosophers as pthreads and start their thread
	threads = malloc(data.nbp  * sizeof(pthread_t));
	
	philos = malloc((data.nbp + 1) * sizeof(t_philo *));
	memset(philos, 0, (data.nbp + 1) * sizeof(t_philo *));
	philos[data.nbp] = NULL;
	
	philos[0] = constructor(&data, 1);
	if (!philos[0])
		return (ERR_MALLOC);
	pthread_create(&threads[0], NULL, &daily_routine, philos[0]);
	
	philos[1]= constructor(&data, 2);
	if (!philos[1])
		return (ERR_MALLOC);
	pthread_create(&threads[1], NULL, &daily_routine, philos[1]);
	
	// Create a new thread that constantly checks wether the philosophers are still alive
	

	// Wait for all the threads in the main process
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	// Exit properly
	pthread_mutex_destroy(&mutexes[0]);
	pthread_mutex_destroy(&mutexes[1]);

	// Free all the memory
	free(mutexes);
	free(threads);
	// free_mutexes(&mutexes);
	// free_threads(threads);
	free_philos(philos);
}
