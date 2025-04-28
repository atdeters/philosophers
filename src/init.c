/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 00:51:57 by andreas           #+#    #+#             */
/*   Updated: 2025/04/29 00:28:53 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	init_prog(t_data *data, int ac, char **av)
{
	if (parse_args(data, ac, av))
		return (data->error);
	data->is_kil = false;
	data->nb_finished_eating = 0;
	data->threads = NULL;
	data->mutexes = NULL;
	data->next_delay = data->nbp;
	return (0);
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
	p->has_finished = false;
	p->mutexes = mutexes;
	if (nb == 1)
		p->fork_left = p->data->nbp - 1;
	else
		p->fork_left = nb - 2;
	p->fork_right = nb - 1;
	return (p);
}

int	allocate_space(t_philo ***philos, t_data *data)
{
	int	i;

	i = 0;
	data->mutexes = malloc((data->nbp + ADD_MUT) * sizeof(pthread_mutex_t));
	if (!data->mutexes)
		return (0);
	data->threads = malloc((data->nbp + 1) * sizeof(pthread_t));
	if (!data->threads)
		return (free_allos(philos, &data->threads, &data->mutexes, 0), 0);
	*philos = malloc((data->nbp + 1) * sizeof(t_philo *));
	if (!*philos)
		return (free_allos(philos, &data->threads, &data->mutexes, 0), 0);
	(*philos)[data->nbp] = NULL;
	while (i < data->nbp)
	{
		(*philos)[i] = constructor(data, data->mutexes, i + 1);
		if (!(*philos)[i])
		{
			free_allos(philos, &data->threads, &data->mutexes, i - 1);
			return (0);
		}
		i++;
	}
	return (p_err(ERR_MALLOC), 1);
}
