/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/24 16:15:06 by adeters          ###   ########.fr       */
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
		// Take smaller fork
		if (p->fork_left < p->fork_right)
			pthread_mutex_lock(&p->mutexes[p->fork_left]);
		else
			pthread_mutex_lock(&p->mutexes[p->fork_right]);
		if (!p_log(p->data, p->philo_nb, FORK, p->mutexes))
		{
			if (p->fork_left < p->fork_right)
				pthread_mutex_unlock(&p->mutexes[p->fork_left]);
			else
				pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break;
		}

		// Take bigger fork
		if (p->fork_left > p->fork_right)
			pthread_mutex_lock(&p->mutexes[p->fork_left]);
		else
			pthread_mutex_lock(&p->mutexes[p->fork_right]);
		if (!p_log(p->data, p->philo_nb, FORK, p->mutexes))
		{
			pthread_mutex_unlock(&p->mutexes[p->fork_left]);
			pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break;
		}

		// Eat and update the time since the last meal
		if (!p_log(p->data, p->philo_nb, EAT, p->mutexes))
		{
			pthread_mutex_unlock(&p->mutexes[p->fork_left]);
			pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break;
		}
		pthread_mutex_lock(&p->mutexes[p->data->nbp + 2]);
		p->time_since_meal = time_passed(p->data);
		pthread_mutex_unlock(&p->mutexes[p->data->nbp + 2]);
		usleep(p->data->tte);
		p->times_eaten++; //? Does not need mutex i think as only one thread uses it

		// Put down both forks at the same time
		pthread_mutex_unlock(&p->mutexes[p->fork_left]);
		pthread_mutex_unlock(&p->mutexes[p->fork_right]);

		// If finished, update the number of philosophers that have finished
		if (p->times_eaten == p->data->nbte)
		{
			p->data->nb_finished_eating++; //! also needs a mutex
			break;
		}

		// Sleep
		if (!p_log(p->data, p->philo_nb, SLEEP, p->mutexes))
			break;
		usleep(p->data->tts);

		// Think
		if (!p_log(p->data, p->philo_nb, THINK, p->mutexes))
			break;
	}
	return (NULL);
}

/**
 * Check whether a certain philosopher from the
 * philos array has died already
 */
bool	is_p_kil(t_philo **ps, int i)
{
	pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 2]);
	if ((time_passed((*ps)->data)
			- ps[i]->time_since_meal) >= (unsigned int)(*ps)->data->ttd / 1000)
		return (pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 2]),
			true);
	return (pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 2]), false);
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
			if (is_p_kil(ps, i))
			{
				pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
				pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp]);
				// p_log((*ps)->data, i + 1, DIE, (*ps)->mutexes);
				printf("%d\t%d died\n", time_passed((*ps)->data), i + 1);
				(*ps)->data->is_kil = true;
				pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp]);
				pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
				flag = true;
				break;
			}
			i++;
		}
		if (flag)
			break;
		if ((*ps)->data->nb_finished_eating == (*ps)->data->nbp)
			break;
		usleep(500);
	}
	return (NULL);
}

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
	if (p->data->nbp != 2)
	{
		if (nb == 1)
			p->fork_left = p->data->nbp - 1;
		else
			p->fork_left = nb - 1;
		if (nb == p->data->nbp)
			p->fork_right = 0;
		else
			p->fork_right = nb;
	}
	else
	{
		if (nb == 1)
		{
			p->fork_left = 1;
			p->fork_right = 0;
		}
		else
		{
			p->fork_left = 0;
			p->fork_right = 1;
		}
	}
	return (p);
}

// Maybe a good idea to make an extra thread for the dead processing and an extra mutex for the 
// Printer to not fuck something up because of their confusing names right now, also death mutex now
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
	// Also create 3 additional mutexes for printing and some checks etc. (aka figure out what exactly is needed)
	// Maybe include mutex pointers in the data struct to give them a proper name (but keep them in the array for performance)
	mutexes = malloc((data.nbp + ADD_MUT) * sizeof(pthread_mutex_t));
	memset(mutexes, 0, (data.nbp + ADD_MUT) * sizeof(pthread_mutex_t));

	int	i;

	i = 0;
	while (i < data.nbp + ADD_MUT)
	{
		pthread_mutex_init(&mutexes[i], NULL);
		i++;
	}

	// Create a list of philosophers as pthreads && add an additional thread for the death checker
	threads = malloc((data.nbp + 1)  * sizeof(pthread_t)); //! Protec + Free
	memset(threads, 0, (data.nbp + 1) * sizeof(pthread_t));
	
	philos = malloc((data.nbp + 1) * sizeof(t_philo *)); //! Protec + Free
	memset(philos, 0, (data.nbp + 1) * sizeof(t_philo *));
	philos[data.nbp] = NULL;

	i = 0;
	while (i < data.nbp)
	{
		philos[i] = constructor(&data, mutexes, i + 1);
		if (!philos[i])
			return (ERR_MALLOC); //! Must free everything
		pthread_create(&threads[i], NULL, &daily_routine, philos[i]); //! Protec
		i++;
	}

	// Create a new thread that constantly checks wether the philosophers are still alive
	pthread_create(&threads[data.nbp], NULL, &check_death, philos); //! Protec
	//! If the creation of a thread fails than all of the previous ones need to be joined before exiting

	// Wait for all the threads in the main process
	i = 0;
	while (i < data.nbp + 1)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	// Exit properly
	i = 0;
	while (i < data.nbp + ADD_MUT)
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
	if (data.is_kil)
		return (1);
	return (0);
}
