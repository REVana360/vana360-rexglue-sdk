# Releases

Pushes and pull requests run repository checks and a Windows SDK build.
They do not publish archives or releases.

The platform release workflows run only for an explicitly created `v*` tag.
Creating or pushing a release tag requires owner approval. The tag must identify
the exact SDK revision intended for downstream Vana360 consumers.

The upstream project publishes its own builds and release notes at
[rexglue/rexglue-sdk releases](https://github.com/rexglue/rexglue-sdk/releases).
