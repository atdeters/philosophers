/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/07 16:29:39 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <stdio.h>

void	*daily_routine(void *philo)
{
	t_philo *p;

	p = (t_philo *)philo;
	while (1)
	{
		// Take a fork
		p_log(p->data, p->philo_nb, FORK);
		// Take another fork
		p_log(p->data, p->philo_nb, FORK);
		// Eat
		p_log(p->data, p->philo_nb, EAT);
		p->time_since_meal = time_passed(p->data);
		usleep(p->data->tte);
		// Sleep
		p_log(p->data, p->philo_nb, SLEEP);
		usleep(p->data->tts);
		// Think
		p_log(p->data, p->philo_nb, THINK);
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
	return (p);
}

int	main(int ac, char **av)
{
	t_data	data;

	if (init_prog(&data, ac, av))
		return (p_err(data.error));
	// Create a list of forks as mutexes
	
	// Start timer
	if (gettimeofday(&data.start, NULL) < 0)
		return (ERR_GTOD);

	// Create a list of philosophers as pthreads and start their thread
	pthread_t p1;
	t_philo *philo1 = constructor(&data, 1);
	if (!philo1)
		return (1);
	pthread_create(&p1, NULL, &daily_routine, philo1);

	pthread_t p2;
	t_philo *philo2 = constructor(&data, 2);
	if (!philo2)
		return (1);
	pthread_create(&p2, NULL, &daily_routine, philo2);
	// Create a new thread that constantly checks wether the philosophers are still alive

	// Wait for all the threads in the main process
	pthread_join(p1, NULL);
}
