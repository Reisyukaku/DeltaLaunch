ARG DEVKITPRO_IMAGE=devkitpro/devkita64:latest
FROM ${DEVKITPRO_IMAGE}
# Upgrade the SDK and dependencies together; never partially upgrade pacman packages.
RUN dkp-pacman -Syu --needed --noconfirm switch-dev switch-glfw switch-mesa switch-freetype switch-sdl2 switch-sdl2_mixer switch-curl \
    && dkp-pacman -Scc --noconfirm
WORKDIR /work
CMD ["make", "switch"]
