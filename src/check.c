/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 01:02:24 by andreas           #+#    #+#             */
/*   Updated: 2025/05/03 17:38:33 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

bool	is_p_kil(t_philo **ps, int i)
{
	unsigned int	passed_time;

	pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 2]);
	passed_time = (time_passed((*ps)->data) - ps[i]->time_since_meal);
	if (passed_time >= (unsigned int)(*ps)->data->ttd / 1000)
		return (pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 2]),
			true);
	return (pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 2]), false);
}

bool	everyone_ate(t_philo **philos)
{
	pthread_mutex_lock(&(*philos)->mutexes[(*philos)->data->nbp + 3]);
	if ((*philos)->data->nb_finished_eating == (*philos)->data->nbp)
	{
		pthread_mutex_unlock(&(*philos)->mutexes[(*philos)->data->nbp + 3]);
		return (true);
	}
	pthread_mutex_unlock(&(*philos)->mutexes[(*philos)->data->nbp + 3]);
	return (false);
}

void	do_on_death(t_philo **ps, int i)
{
	pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
	pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp]);
	printf("%d\t%d died\n", time_passed((*ps)->data), i + 1);
	pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp]);
	(*ps)->data->is_kil = true;
	pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 1]);
}

bool	is_finish(t_philo **ps, int i)
{
	pthread_mutex_lock(&(*ps)->mutexes[(*ps)->data->nbp + 4]);
	if (ps[i]->has_finished)
	{
		pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 4]);
		return (true);
	}
	return (pthread_mutex_unlock(&(*ps)->mutexes[(*ps)->data->nbp + 4]), false);
}

void	*death_thread(void *philos)
{
	t_philo	**ps;
	int		i;

	ps = (t_philo **)philos;
	usleep(50000);
	while (1)
	{
		i = 0;
		while (i < (*ps)->data->nbp)
		{
			if (!is_finish(ps, i) && is_p_kil(ps, i))
			{
				do_on_death(ps, i);
				return (NULL);
			}
			i++;
		}
		if (is_over(ps) || everyone_ate(ps))
			break ;
		usleep(500);
	}
	return (NULL);
}
