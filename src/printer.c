/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adeters <adeters@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:14:34 by adeters           #+#    #+#             */
/*   Updated: 2025/04/07 16:52:05 by adeters          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	p_str_fd(int fd, char *str)
{
	write(fd, str, ft_strlen(str));
}

bool	p_log(t_data *data, int philo_nb, int action)
{
	if (data->is_kil)
		return (false);
	p_nbr_fd(1, time_passed(data));
	p_str_fd(1, "\t");
	p_nbr_fd(1, philo_nb);
	if (action == FORK)
		p_str_fd(1, " has taken fork\n");
	if (action == EAT)
		p_str_fd(1, " is eating\n");
	if (action == SLEEP)
		p_str_fd(1, " is sleeping\n");
	if (action == THINK)
		p_str_fd(1, " is thinking\n");
	if (action == DIE)
		p_str_fd(1, " died\n");
	return (true);
}

int	p_err(int code)
{
	if (code == ERR_USAGE)
		p_str_fd(2, "Usage: ./philosopher nb ttd tte tts [nbte]\n");
	if (code == ERR_GTOD)
		p_str_fd(2, "gettimeofday function failed to execute\n");
	return (code);
}

void	p_err_form(char *msg, char *arg)
{
	p_str_fd(2, msg);
	p_str_fd(2, "\'");
	p_str_fd(2, arg);
	p_str_fd(2, "\'");
	p_str_fd(2, "\n");
}

int	p_err_arg(int code, char *arg)
{
	p_str_fd(2, "Error: ");
	if (code == ERR_INV_ARG)
		p_err_form("Invalid argument: ", arg);
	if (code == ERR_OVERFLOW)
		p_err_form("Argument causes integer overflow: ", arg);
	if (code == ERR_UNDERFLOW)
		p_err_form("Argument causes integer underflow: ", arg);
	if (code == ERR_NEG_ARG)
		p_err_form("Argument can not be negative: ", arg);
	return (code);
}
