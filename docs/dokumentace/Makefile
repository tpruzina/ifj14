NAME=dokumentace

$(NAME).pdf: $(NAME).dvi
	dvipdf $(NAME).dvi

$(NAME).dvi: $(NAME).tex title.tex
	latex $(NAME).tex
clean:
	rm -f $(NAME).pdf $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out
