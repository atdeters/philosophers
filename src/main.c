/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/07 17:01:08 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <stdio.h>

//! Needs to stop immideatily when one of the philosopher dies
void	*daily_routine(void *philo)
{
	t_philo *p;

	p = (t_philo *)philo;
	while (1)
	{
		if (!p_log(p->data, p->philo_nb, FORK))
			break;
		if (!p_log(p->data, p->philo_nb, FORK))
			break;
		if (!p_log(p->data, p->philo_nb, EAT))
			break;
		p->time_since_meal = time_passed(p->data);
		usleep(p->data->tte);
		if (!p_log(p->data, p->philo_nb, SLEEP))
			break;
		usleep(p->data->tts);
		if (!p_log(p->data, p->philo_nb, THINK))
			break;
	}
	return (NULL);
}

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
	p->fork_left = nb;
	p->fork_right = nb + 1; //! This needs to wrap around
	return (p);
}

int	main(int ac, char **av)
{
	t_data			data;
	// t_philo			**philos; // TODO: Make this a list/array for all the philosophers
	// pthread_t		*threads; // TODO: Make this a list of all the threads
	// pthread_mutex_t *mutexes; // TODO: Make this a list of all the mutexes;

	if (init_prog(&data, ac, av))
		return (p_err(data.error));

	// Start timer
	if (gettimeofday(&data.start, NULL) < 0)
		return (ERR_GTOD);
	
	// Create a list of forks as mutexes
	pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&m1, NULL);
	pthread_mutex_init(&m2, NULL);


	// Create a list of philosophers as pthreads and start their thread
	pthread_t pth1;
	t_philo *philo1 = constructor(&data, 1);
	if (!philo1)
		return (1);
	pthread_create(&pth1, NULL, &daily_routine, philo1);


	// pthread_t pth2;
	// t_philo *philo2 = constructor(&data, 2);
	// if (!philo2)
	// 	return (1);
	// pthread_create(&pth2, NULL, &daily_routine, philo2);

	// Create a new thread that constantly checks wether the philosophers are still alive

	// Wait for all the threads in the main process
	pthread_join(pth1, NULL);
	// pthread_join(pth2, NULL);
}
