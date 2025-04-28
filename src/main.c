/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/29 00:46:53 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <stdio.h>

int		get_next_delay(t_data *data)
{
	int res;

	pthread_mutex_lock(&data->mutexes[data->nbp + 5]);
	res = data->next_delay;
	return (pthread_mutex_unlock(&data->mutexes[data->nbp + 5]), res);
}

void	*daily_routine(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	while (1)
	{
		
		
		
		//============= FORK 1 Block
		if (p->data->ttd == 0)
			break ;
		// Force odd to start the race
		if (p->philo_nb % 2 == 0 && p->times_eaten == 0)
			usleep(20000);
		if (p->data->nbp % 2 != 0 && p->philo_nb == get_next_delay(p->data))
		{
			usleep(20000);
			pthread_mutex_lock(&p->data->mutexes[p->data->nbp + 5]);
			if (p->philo_nb == p->data->nbp)
				p->data->next_delay = 1;
			else
				p->data->next_delay += 2;
			pthread_mutex_unlock(&p->data->mutexes[p->data->nbp + 5]);
		}
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
			break ;
		}
		//=======================
		
		
		//====================== FORK 2 Block
		if (p->data->nbp == 1)
		{
			usleep(p->data->ttd);
			pthread_mutex_unlock(&p->mutexes[0]);
			break ;
		}
		if (p->philo_nb % 2 != 0)
			pthread_mutex_lock(&p->mutexes[p->fork_left]);
		else
			pthread_mutex_lock(&p->mutexes[p->fork_right]);
		if (!p_log(p->data, p->philo_nb, FORK, p->mutexes))
		{
			pthread_mutex_unlock(&p->mutexes[p->fork_left]);
			pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break ;
		}
		//=======================


		//======================= EATING BLOCK
		if (!p_log(p->data, p->philo_nb, EAT, p->mutexes))
		{
			pthread_mutex_unlock(&p->mutexes[p->fork_left]);
			pthread_mutex_unlock(&p->mutexes[p->fork_right]);
			break ;
		}
		pthread_mutex_lock(&p->mutexes[p->data->nbp + 2]);
		p->time_since_meal = time_passed(p->data);
		pthread_mutex_unlock(&p->mutexes[p->data->nbp + 2]);
		usleep(p->data->tte);
		p->times_eaten++;
		pthread_mutex_unlock(&p->mutexes[p->fork_left]);
		pthread_mutex_unlock(&p->mutexes[p->fork_right]);
		if (p->times_eaten == p->data->nbte)
		{
			pthread_mutex_lock(&p->mutexes[p->data->nbp + 4]);
			p->has_finished = true;
			pthread_mutex_unlock(&p->mutexes[p->data->nbp + 4]);
			pthread_mutex_lock(&p->mutexes[p->data->nbp + 3]);
			p->data->nb_finished_eating++;
			pthread_mutex_unlock(&p->mutexes[p->data->nbp + 3]);
			break ;
		}
		//==========================



		

		//============== Think & Sleep Block
		if (!p_log(p->data, p->philo_nb, SLEEP, p->mutexes))
			break ;
		usleep(p->data->tts);
		if (!p_log(p->data, p->philo_nb, THINK, p->mutexes))
			break ;
		//======================
	}
	return (NULL);
}

int	main(int ac, char **av)
{
	int		i;
	t_data	data;
	t_philo	**philos;

	philos = NULL;
	if (init_prog(&data, ac, av))
		return (p_err(data.error));
	if (data.nbte == 0)
		return (0);
	if (gettimeofday(&data.start, NULL) < 0)
		return (ERR_GTOD);
	if (!allocate_space(&philos, &data))
		return (ERR_MALLOC);
	// Creation of the mutexes
	i = 0;
	while (i < data.nbp + ADD_MUT)
	{
		if (pthread_mutex_init(&data.mutexes[i], NULL))
		{
			destroy_mutex_nb(data.mutexes, i);
			free_allos(&philos, &data.threads, &data.mutexes, data.nbp);
			return (p_err(ERR_MUT_INIT));
		}
		i++;
	}
	// Creation of the threads
	i = 0;
	pthread_mutex_lock(&data.mutexes[data.nbp + 1]);
	while (i < data.nbp)
	{
		if (pthread_create(&data.threads[i], NULL, &daily_routine, philos[i]))
		{
			p_err(ERR_THREAD_CREATE);
			data.is_kil = true;
			pthread_mutex_unlock(&data.mutexes[data.nbp + 1]);
			destroy_threads_nb(data.threads, i);
			destroy_mutex_nb(data.mutexes, data.nbp + ADD_MUT);
			free_allos(&philos, &data.threads, &data.mutexes, data.nbp);
			return (ERR_THREAD_CREATE);
		}
		i++;
	}
	if (pthread_create(&data.threads[data.nbp], NULL, &death_thread, philos))
	{
		p_err(ERR_THREAD_CREATE);
		data.is_kil = true;
		pthread_mutex_unlock(&data.mutexes[data.nbp + 1]);
		destroy_threads_nb(data.threads, data.nbp + 1);
		destroy_mutex_nb(data.mutexes, data.nbp + ADD_MUT);
		free_allos(&philos, &data.threads, &data.mutexes, data.nbp);
		return (ERR_THREAD_CREATE);
	}
	pthread_mutex_unlock(&data.mutexes[data.nbp + 1]);
	// Exit program cleanly
	destroy_threads_nb(data.threads, data.nbp + 1);
	destroy_mutex_nb(data.mutexes, data.nbp + ADD_MUT);
	free_allos(&philos, &data.threads, &data.mutexes, data.nbp);
	if (data.is_kil)
		return (ERR_IS_KIL);
	return (0);
}
