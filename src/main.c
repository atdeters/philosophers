/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/08 12:59:14 by andreas          ###   ########.fr       */
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

void	*check_death(void *philos)
{
	t_philo	**ps;
	int		i;
	int		nb;
	bool	flag;

	ps = (t_philo **)philos;
	nb = (*ps)->data->nbp;
	flag = false;
	while (1)
	{
		i = 0;
		while (i < nb)
		{
			if ((time_passed((*ps)->data) - ps[i]->time_since_meal) > (unsigned int)(*ps)->data->ttd / 1000)
			{
				p_log((*ps)->data, i + 1, DIE);
				(*ps)->data->is_kil = true;
				flag = true;
				break;
			}
			i++;
		}
		if (flag)
			break;
	}
	return (NULL);
}

// Make sure to not fuck up 0-Indexing while 1-Indexing philosophers
t_philo	*constructor(t_data *data, pthread_mutex_t *mutexes, int nb)
{
	t_philo	*p;

	p = malloc(sizeof(t_philo));
	if (!p)
		return (NULL);
	p->data = data;
	p->philo_nb = nb;
	p->time_since_meal = 0;
	p->times_eaten = 0;
	p->mutexes = mutexes;
	if (nb == 1)
		p->fork_left = p->data->nbp;
	else
		p->fork_left = nb;
	if (nb == p->data->nbp - 1)
		p->fork_right = 1;
	else
		p->fork_right = nb + 1;
	return (p);
}

// TODO: MAke all the allocations happen automatically from the actual number of philos in the cmd line
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
	memset(mutexes, 0, data.nbp * sizeof(pthread_mutex_t));
	
	pthread_mutex_init(&mutexes[0], NULL);
	pthread_mutex_init(&mutexes[1], NULL);

	// Create a list of philosophers as pthreads and start their thread
	threads = malloc((data.nbp + 1)  * sizeof(pthread_t)); //! Protec + Free
	memset(threads, 0, (data.nbp + 1) * sizeof(pthread_t));
	
	philos = malloc((data.nbp + 1) * sizeof(t_philo *)); //! Protec + Free
	memset(philos, 0, (data.nbp + 1) * sizeof(t_philo *));
	philos[data.nbp] = NULL;
	
	int	i;

	i = 0;
	while (i < data.nbp)
	{
		philos[i] = constructor(&data, mutexes, i + 1);
		if (!philos[i])
			return (ERR_MALLOC); //! Must free everything
		pthread_create(&threads[i], NULL, &daily_routine, philos[i]);
		i++;
	}
	
	// Create a new thread that constantly checks wether the philosophers are still alive
	pthread_create(&threads[data.nbp], NULL, &check_death, philos);

	// Wait for all the threads in the main process
	i = 0;
	while (i < data.nbp + 1)
	{
		pthread_join(threads[i], NULL);
		i++;
	}

	// Exit properly
	i = 0;
	while (i < data.nbp)
	{
		pthread_mutex_destroy(&mutexes[i]);
		i++;
	}

	// Free all the memory
	free(mutexes);
	mutexes = NULL;
	free(threads);
	threads = NULL;
	free_philos(philos);
	philos = NULL;
}
