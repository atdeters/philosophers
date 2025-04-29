/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:52 by adeters           #+#    #+#             */
/*   Updated: 2025/04/29 02:57:17 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void	*daily_routine(void *philo)
{
	t_philo	*p;

	p = (t_philo *)philo;
	while (1)
	{
		if (p->data->ttd == 0)
			break ;
		act_manipulate_think(p);
		if (!act_take_fork_one(p))
			break ;
		if (!act_take_fork_two(p))
			break ;
		if (!act_eat(p))
			break ;
		if (!act_think_and_sleep(p))
			break ;
	}
	return (NULL);
}

int	create_philo_threads(t_data *data, t_philo **philos)
{
	int		i;

	i = 0;
	while (i < data->nbp)
	{
		if (pthread_create(&data->threads[i], NULL, &daily_routine, philos[i]))
		{
			p_err(ERR_THREAD_CREATE);
			data->is_kil = true;
			pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
			destroy_threads_nb(data->threads, i);
			destroy_mutex_nb(data->mutexes, data->nbp + ADD_MUT);
			free_allos(&philos, &data->threads, &data->mutexes, data->nbp);
			return (ERR_THREAD_CREATE);
		}
		i++;
	}
	return (0);
}

int	create_death_thread(t_data *data, t_philo **philos)
{
	if (pthread_create(&data->threads[data->nbp], NULL, &death_thread, philos))
	{
		p_err(ERR_THREAD_CREATE);
		data->is_kil = true;
		pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
		destroy_threads_nb(data->threads, data->nbp + 1);
		destroy_mutex_nb(data->mutexes, data->nbp + ADD_MUT);
		free_allos(&philos, &data->threads, &data->mutexes, data->nbp);
		return (ERR_THREAD_CREATE);
	}
	return (0);
}

int	make_threads_and_time(t_data *data, t_philo **philos)
{
	pthread_mutex_lock(&data->mutexes[data->nbp + 1]);
	if (create_philo_threads(data, philos))
	{
		pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
		return (data->error = ERR_THREAD_CREATE, ERR_THREAD_CREATE);
	}
	
	if (gettimeofday(&data->start, NULL) < 0)
	{
		pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
		return (data->error = ERR_GTOD, ERR_GTOD);
	}
	if (create_death_thread(data, philos))
	{
		pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
		return (data->error = ERR_THREAD_CREATE, ERR_THREAD_CREATE);
	}
	pthread_mutex_unlock(&data->mutexes[data->nbp + 1]);
	return (0);
}

int	main(int ac, char **av)
{
	t_data	data;
	t_philo	**philos;

	philos = NULL;
	if (init_prog(&data, ac, av))
		return (p_err(data.error));
	if (data.nbte == 0)
		return (0);
	if (!allocate_space(&philos, &data))
		return (ERR_MALLOC);
	if (create_mutexes(&data, philos))
		return (ERR_MUT_INIT);
	if (make_threads_and_time(&data, philos))
		return (data.error);
	destroy_threads_nb(data.threads, data.nbp + 1);
	destroy_mutex_nb(data.mutexes, data.nbp + ADD_MUT);
	free_allos(&philos, &data.threads, &data.mutexes, data.nbp);
	if (data.is_kil)
		return (ERR_IS_KIL);
	return (0);
}
