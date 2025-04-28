/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 01:34:34 by andreas           #+#    #+#             */
/*   Updated: 2025/04/29 01:50:37 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void	act_manipulate_think(t_philo *p)
{
	if (p->philo_nb % 2 == 0 && p->times_eaten == 0)
	usleep(30000);
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
}

// int	act_take_fork_one(t_philo *p)
// {

// }

// int	act_take_fork_two(t_philo *p)
// {

// }

// int	act_eat(t_philo *p)
// {

// }

int	act_think_and_sleep(t_philo *p)
{
	if (!p_log(p->data, p->philo_nb, SLEEP, p->mutexes))
		return (0);
	usleep(p->data->tts);
	if (!p_log(p->data, p->philo_nb, THINK, p->mutexes))
		return (0);
	return (1);
}
