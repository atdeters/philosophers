/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andreas <andreas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 16:03:45 by adeters           #+#    #+#             */
/*   Updated: 2025/04/26 13:10:54 by andreas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

// LIBRARIES
# include <sys/time.h>
# include <limits.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdbool.h>
# include <pthread.h>
# include <stdio.h>
# include <string.h>

// Macros
/**
 * Defines how many mutexes are needed in addition to the forks
 */
# define ADD_MUT 3

// ENUMS
enum				e_errors
{
	/**
	* @brief The program was not executed correctly (e.g. faulty arguments)
	*/
	ERR_USAGE = 1,
	/**
	 * @brief gettimeofday function failed to execute
	 */
	ERR_GTOD = 2,
	ERR_INV_ARG = 3,
	ERR_OVERFLOW = 4,
	ERR_UNDERFLOW = 5,
	ERR_NEG_ARG = 6,
	ERR_MALLOC = 12,
	ERR_IS_KILL = 42,
};

enum				e_actions
{
	FORK,
	EAT,
	SLEEP,
	THINK,
	DIE,
};

// STRUCTS
typedef struct s_data
{
	/**
		* @brief number_of_philosophers
		*
		* The number of philosophers and also the number of forks.
		*/
	int				nbp;
	/**
		* @brief  time_to_die (in milliseconds)
		*
		* If a philosopher didn’t start eating time_to_die milliseconds
		* since the beginning of their last meal or the beginning of
		* the simulation, they die.
		*/
	int				ttd;
	/**
		* @brief time_to_eat (in milliseconds)
		*
		* The time it takes for a philosopher to eat.
		*
		* During that time, they will need to hold two forks.
		*/
	int				tte;
	/**
		* @brief time_to_sleep (in milliseconds)
		*
		* The time a philosopher will spend sleeping.
		*/
	int				tts;
	/**
		* @brief number_of_times_each_philosopher_must_eat (optional argument)
		* If all philosophers have eaten at least
		* number_of_times_each_philosopher_must_eat times, the simulation stops.
		* If not specified, the simulation stops when a philosopher dies.
		*/
	int				nbte;
	bool			is_kil;
	int				error;
	int				nb_finished_eating;
	struct timeval	start;
	struct timeval	curr;
	unsigned int	elapsed;
}					t_data;

typedef struct	s_philo
{
	t_data			*data;
	unsigned int	time_since_meal;
	int				philo_nb;
	int				times_eaten;
	int				fork_left;
	int				fork_right;
	pthread_mutex_t	*mutexes;
}				t_philo;


typedef struct s_atoi_data
{
	int	sign;
	int	nmb;
	int	i;
}				t_atoi_data;

// FUNCTIONS
// check_overflow.c
/**
 * @brief Determines if a string representation of a number
 * will overflow or underflow a 32-bit signed integer.
 *
 * This function evaluates whether converting a numeric string
 * to an integer using `ft_atoi()` would cause an overflow or
 * underflow of a 32-bit signed integer. It considers the maximum
 * and minimum values of `INT_MAX` (2147483647) and `INT_MIN` (-2147483648).
 *
 * The function accepts strings with optional `+` or `-` signs and ignores
 * leading zeros before the number. If the string is invalid (e.g., contains
 * no digits after the optional sign or zeros),
 * the function returns an error code.
 *
 * @param str The string containing the number to evaluate.
 * The string must be null-terminated and may include:
 *
 *            - An optional `+` or `-` as the first character.
 *
 *            - Leading zeros, which are ignored during evaluation.
 *
 * @return
 * - `0` if the number fits within the range of a 32-bit signed integer.
 *
 * - `-1` if the number would cause an underflow (less than `INT_MIN`).
 *
 * - `1` if the number would cause an overflow (greater than `INT_MAX`).
 *
 * - `-2` if the string is invalid (e.g., contains non-numeric
 * characters, or no digits after `+` or `-`).
 */
int					check_overflow(char *str);

// free.c
void				free_philos(t_philo **lst);

// helpers.c
int					ft_strlen(char *str);
void				p_nbr_fd(int fd, int n);
/**
 * @brief Converts a string to an integer.
 *
 * This function parses the initial portion of the string pointed to by @p nptr
 * and converts it to an integer, taking into account leading whitespace and an
 * optional sign. The conversion stops at the first non-numeric character after
 * the digits, if any. If the string does not contain any valid integer
 * representation, it returns -1.
 *
 * @param nptr A pointer to the string to be converted.
 * @return The converted integer from the string. If @p nptr is NULL or the
 * string does not start with a number, returns -1.
 *
 * @note The function does not check for integer overflow or underflow.
 *
 *
 * `int value = ft_atoi("  -123abc");` -> value will be -123
 *
 */
int					ft_atoi(const char *nptr);
unsigned int		time_passed(t_data *data);

// helpers2.c
int					ft_isdigit(int c);
int					ft_strncmp(const char *s1, const char *s2, size_t n);
void				free_mutexes(pthread_mutex_t **lst);
void				free_threads(pthread_t **lst);
void				free_philos(t_philo **lst);

// init.c
int					check_arg(t_data *data, char *input, int *value, bool is_time);
int					parse_args(t_data *data, int ac, char **av);
int					init_prog(t_data *data, int ac, char **av);

// printer.c
/**
 * @brief Prints a string str to the file descriptor fd
 */
void				p_str_fd(int fd, char *str);
/**
 * @brief Prints a log for every action of any philosopher
 */
bool				p_log(t_data *data, int philo_nb, int action, pthread_mutex_t *mutexes);
int					p_err(int code);
int					p_err_arg(int code, char *arg);

#endif