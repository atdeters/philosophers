/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/26 14:54:42 by andreas          ###   ########.fr       */
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
		// Don't do anything for a ttd of 0
		if (p->data->ttd == 0)
			break;

		// Take first fork depending on philosopher number
		if (p->philo_nb % 2 == 0)
			pthread_mutex_lock(&p->mutexes[p->fork_left]);
		else
			pthread_mutex_lock(&p->mutexes[p->fork_right]);
		if (!p_log(p->data, p->philo_nb, FORK, p->mutexes))
		{
			if (p->philo_nb % 2 == 0)
				pthread_mutex_unlock(&p->mutexes[p->fork_left]);
			else
				pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break;
		}

		// Special case for only having one philosopher
		if (p->data->nbp == 1)
		{
			usleep(p->data->ttd);
			pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break;
		}

		// Take second fork depending on philosopher number
		if (p->philo_nb % 2 != 0)
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
		p->times_eaten++;

		// Put down both forks at the same time
		pthread_mutex_unlock(&p->mutexes[p->fork_left]);
		pthread_mutex_unlock(&p->mutexes[p->fork_right]);

		// If finished, update the number of philosophers that have finished
		if (p->times_eaten == p->data->nbte)
		{
			pthread_mutex_lock(&p->mutexes[p->data->nbp + 3]);
			p->data->nb_finished_eating++;
			pthread_mutex_unlock(&p->mutexes[p->data->nbp + 3]);
			break;
		}

		// Sleep
		if (!p_log(p->data, p->philo_nb, SLEEP, p->mutexes))
			break;
		usleep(p->data->tts);

		// Think
		usleep(1000);
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

bool	everyone_ate(t_philo **philos)
{
	pthread_mutex_lock(&(*philos)->mutexes[(*philos)->data->nbp + 3]);
	if ((*philos)->data->nb_finished_eating == (*philos)->data->nbp)
		return (pthread_mutex_unlock(&(*philos)->mutexes[(*philos)->data->nbp + 3]), true);
	return (pthread_mutex_unlock(&(*philos)->mutexes[(*philos)->data->nbp + 3]), false);
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
				pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp]);
				// p_log((*ps)->data, i + 1, DIE, (*ps)->mutexes);
				printf("%d\t%d died\n", time_passed((*ps)->data), i + 1);
				pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp]);
				pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
				(*ps)->data->is_kil = true;
				pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
				flag = true;
				break;
			}
			i++;
		}
		if (flag)
			break;
		if (everyone_ate(ps))
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
	if (nb == 1)
		p->fork_left = p->data->nbp - 1;
	else
		p->fork_left = nb - 2;
	p->fork_right = nb - 1;
	return (p);
}

void	free_allos(t_philo ***philos, pthread_t **threads, pthread_mutex_t **mutexes, int nbp)
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

int	main(int ac, char **av)
{
	t_data			data;
	int				i;
	//TODO:  Make these part of the data structure so I can easier call their free func and init them
	t_philo			**philos;
	pthread_t		*threads;
	pthread_mutex_t *mutexes;

	philos = NULL;
	threads = NULL;
	mutexes = NULL;
	
	if (init_prog(&data, ac, av))
		return (p_err(data.error));

	// Start timer
	if (gettimeofday(&data.start, NULL) < 0)
		return (ERR_GTOD);

	// All the mallocs
	mutexes = malloc((data.nbp + ADD_MUT) * sizeof(pthread_mutex_t));
	if (!mutexes)
		return (ERR_MALLOC);
	memset(mutexes, 0, (data.nbp + ADD_MUT) * sizeof(pthread_mutex_t));
	threads = malloc((data.nbp + 1)  * sizeof(pthread_t));
	if (!threads)
		return (free_allos(&philos, &threads, &mutexes, 0), ERR_MALLOC);
	memset(threads, 0, (data.nbp + 1) * sizeof(pthread_t));
	philos = malloc((data.nbp + 1) * sizeof(t_philo *));
	if (!philos)
		return (free_allos(&philos, &threads, &mutexes, 0), ERR_MALLOC);
	memset(philos, 0, (data.nbp + 1) * sizeof(t_philo *));
	philos[data.nbp] = NULL;
	i = 0;
	while (i < data.nbp)
	{
		philos[i] = constructor(&data, mutexes, i + 1);
		if (!philos[i])
			return (free_allos(&philos, &threads, &mutexes, i - 1), ERR_MALLOC);
		i++;
	}

	// Creation of the mutexes
	i = 0;
	while (i < data.nbp + ADD_MUT)
	{
		pthread_mutex_init(&mutexes[i], NULL); //! Protec
		i++;
	}

	// Creation of the threads
	i = 0;
	while (i < data.nbp)
	{
		pthread_create(&threads[i], NULL, &daily_routine, philos[i]); //! Protec
		i++;
	}
	pthread_create(&threads[data.nbp], NULL, &check_death, philos); //! Protec

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
	free_allos(&philos, &threads, &mutexes, data.nbp);
	if (data.is_kil)
		return (ERR_IS_KIL);
	return (0);
}
