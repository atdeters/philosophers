/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/07 15:25:06 by adeters          ###   ########.fr       */
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



int	main(int ac, char **av)
{
	t_data	data;

	if (init_prog(&data, ac, av))
		return (p_err(data.error));
	if (gettimeofday(&data.start, NULL) < 0)
		return (ERR_GTOD);

	// Create a list of philosophers as pthreads and start their thread
	pthread_t p1;
	t_philo philo1;
	philo1.data = &data;
	philo1.philo_nb = 1;
	philo1.time_since_meal = 0;
	pthread_create(&p1, NULL, &daily_routine, &philo1);

	pthread_t p2;
	t_philo philo2;
	philo2.data = &data;
	philo2.philo_nb = 2;
	philo2.time_since_meal = 0;
	pthread_create(&p2, NULL, &daily_routine, &philo2);
	
	// Create a list of forks as mutexes

	// Create a new thread that constantly checks wether the philosophers are still alive

	// Wait for all the threads in the main process
	pthread_join(p1, NULL);
}
