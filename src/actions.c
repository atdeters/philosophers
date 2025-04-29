/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 01:34:34 by andreas           #+#    #+#             */
/*   Updated: 2025/04/29 02:31:44 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosophers.h"

void	act_manipulate_think(t_philo *p)
{
	if (p->philo_nb % 2 == 0 && p->times_eaten == 0)
		usleep(30000);
	if (p->data->nbp % 2 != 0 && p->philo_nb == get_next_delay(p->data))
	{
		if (p->data->nbp == 1 || p->data->nbp == 2)
			return ;
		usleep(20000);
		pthread_mutex_lock(&p->data->mutexes[p->data->nbp + 5]);
		if (p->philo_nb == p->data->nbp)
			p->data->next_delay = 1;
		else
			p->data->next_delay += 2;
		pthread_mutex_unlock(&p->data->mutexes[p->data->nbp + 5]);
	}
}

int	act_take_fork_one(t_philo *p)
{
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
		return (0);
	}
	return (1);
}

//TODO: Explain the killing of a single philo in the docs
int	act_take_fork_two(t_philo *p)
{
	if (p->data->nbp == 1)
	{
		usleep(p->data->ttd);
		pthread_mutex_unlock(&p->mutexes[0]);
		return (0);
	}
	if (p->philo_nb % 2 != 0)
		pthread_mutex_lock(&p->mutexes[p->fork_left]);
	else
		pthread_mutex_lock(&p->mutexes[p->fork_right]);
	if (!p_log(p->data, p->philo_nb, FORK, p->mutexes))
	{
		pthread_mutex_unlock(&p->mutexes[p->fork_left]);
		pthread_mutex_unlock(&p->mutexes[p->fork_right]);
		return (0);
	}
	return (1);
}

int	act_eat(t_philo *p)
{
	if (!p_log(p->data, p->philo_nb, EAT, p->mutexes))
	{
		pthread_mutex_unlock(&p->mutexes[p->fork_left]);
		pthread_mutex_unlock(&p->mutexes[p->fork_right]);
		return (0);
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
		return (0);
	}
	return (1);
}

int	act_think_and_sleep(t_philo *p)
{
	if (!p_log(p->data, p->philo_nb, SLEEP, p->mutexes))
		return (0);
	usleep(p->data->tts);
	if (!p_log(p->data, p->philo_nb, THINK, p->mutexes))
		return (0);
	return (1);
}
